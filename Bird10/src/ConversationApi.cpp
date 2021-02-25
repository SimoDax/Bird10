/*
* Bird10
* Copyright (C) 2020  Simone Dassi
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "ConversationApi.hpp"
#include <src/QJson4/QJsonObject.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/TimelineDataModel.hpp>

ConversationApi::ConversationApi(QObject *parent) : TwitterApi(parent)
{
}

void ConversationApi::loadConversation(const QString& id, bool includeCursor)
{
    m_id = id;
    m_mainTweetIndex = -1;
    QString request("https://api.twitter.com/2/timeline/conversation/" + id + ".json?include_profile_interstitial_type=1&include_blocking=1&include_blocked_by=1&include_followed_by=1&include_want_retweets=1&include_mute_edge=1&include_can_dm=1&include_can_media_tag=1&skip_status=1&cards_platform=Web-12&include_cards=1&include_composer_source=true&include_ext_alt_text=true&include_reply_count=1&tweet_mode=extended&include_entities=true&include_user_entities=false&include_ext_media_color=false&include_ext_media_availability=false&send_error_codes=true&simple_quoted_tweets=true&count=50&ext=mediaStats,highlightedLabel,cameraMoment");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("include_user_entities", "false"));
    par.append(O0RequestParameter("count", "100"));

    if(includeCursor){
        if(!tweetModel_->cursorBottom().isEmpty()){
            /*
             * Twitter api for conversations doesn't have two separate parameters for top and bottom cursor, so it works in this way:
             * - if you send the cursor-top contained in a previous reply, the server will return at most <count> tweets towards the head of the thread
             * - if you send the cursor-bottom contained in a previous reply, the server will return:
             *      - at most <count> thread tweets towards the end of the thread
             *      - if there are less than count, it will begin returning replies to the thread
             *      - when all replies have been returned, cursor-bottom won't be present in the json anymore (this is inconsistent with other
             *          endpoints which use a terminateTimeline entry to signal that there's no more content to retrieve)
             */

            // For now only additional tweets towards the bottom are retrieved
            par.append(O0RequestParameter("cursor", tweetModel_->cursorBottom().toUtf8()));
        }
        else return;
    }

    CurlEasy* reply = requestor->get(request, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(conversationReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ConversationApi::insertTweet(const QVariantMap& tweet)
{
    m_tweetsCount++;
    QString id = tweet["item"].toMap()["content"].toMap()["tweet"].toMap()["id"].toString();

    //TODO: if it's not contained either we're blocked or the account is private, handle this
//    if(!m_tweets.keys().contains(id))
//        return;

    QVariantMap tweetObject = parseTweetV2((m_tweets[id].toMap()), m_tweets, m_users);

    // Mark main tweet
    if(id == m_id){
        tweetObject["main_flag"] = true;
        m_mainTweetIndex = m_tweetsCount - 1;
    }
    else
        tweetObject["main_flag"] = false;

    tweetModel_->append(tweetObject);
}

void ConversationApi::insertTweetsFromThread(const QVariantMap& thread)
{
    QVariantList items = thread["timelineModule"].toMap()["items"].toList();
    for(int i = 0; i<items.size(); i++){
        QStringList ids = items[i].toMap()["entryId"].toString().split("-");

        if(ids[0].contains("tweet"))
            insertTweet(items[i].toMap());
    }
}

void ConversationApi::loadMore()
{
    if(tweetModel_->isTerminateBottom())
        return;

    loadConversation(m_id, true);
}

void ConversationApi::conversationReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    m_users = content["globalObjects"].toMap()["users"].toMap();
    m_tweets = content["globalObjects"].toMap()["tweets"].toMap();

    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();

    // The api returns the first tweet of the conversation even when retrieving older tweets. The id of this tweet coincides with the id of the conversation
    // so we gotta extract it from the reply and use it in the parsing loop below to filter out this misplaced tweet.
    QStringList ids = content["timeline"].toMap()["id"].toString().split("-");
    QString conversationId = ids.size() > 1 ? ids[1] : "";

    tweetModel_->setCursorBottom("");
    m_tweetsCount = 0;
    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().keys().contains("addEntries")){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
            qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
//                qDebug()<<"entry: "<<i<<" of "<<entries.size();
                QStringList ids = entries[i].toMap()["entryId"].toString().split("-");

                if(ids[0].contains("tweet") && !ids[0].contains("Composer") && ids[1] != conversationId)   // - to avoid tweetComposer on self threads and the first tweet of the converstion
                    insertTweet(entries[i].toMap()["content"].toMap());
                else if (ids[0].contains("Thread"))
                    insertTweetsFromThread(entries[i].toMap()["content"].toMap());
                else if(ids[0].contains("cursor") && (ids[1].contains("bottom") || ids[1].contains("showMoreThreads")))
                    tweetModel_->setCursorBottom(entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString());
                //TODO: implement cursor top
            }
        }
        if(instructions[i].toMap().keys().contains("terminateTimeline")){
            terminateTimeline(instructions[i].toMap());
        }
    }

    /*
     * Workaround: Twitter api's seem to have bugged recently - they don't return any terminateTimeline in the bottom direction, at least for conversations
     * When there are no more replies, however, no cursor is returned. The code that follows exploits this fact to artifically terminate the timeline
     */
    if(tweetModel_->cursorBottom() == "")
        tweetModel_->setTerminateBottom(true);


    emit tweetModelChanged();
    if(m_mainTweetIndex != -1)
        emit conversationLoaded(m_mainTweetIndex);  // if the loaded piece of conversation contains the main tweet then notify its position to qml

    m_users.clear();
    m_tweets.clear();

    reply->deleteLater();
}
