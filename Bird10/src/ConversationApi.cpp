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
    tweetModel_ = new TimelineDataModel(parent);
}

void ConversationApi::loadConversation(const QString& id, bool includeCursor)
{
    m_id = id;
    QString request("https://api.twitter.com/2/timeline/conversation/" + id + ".json?include_profile_interstitial_type=1&include_blocking=1&include_blocked_by=1&include_followed_by=1&include_want_retweets=1&include_mute_edge=1&include_can_dm=1&include_can_media_tag=1&skip_status=1&cards_platform=Web-12&include_cards=1&include_composer_source=true&include_ext_alt_text=true&include_reply_count=1&tweet_mode=extended&include_entities=true&include_user_entities=false&include_ext_media_color=false&include_ext_media_availability=false&send_error_codes=true&simple_quoted_tweets=true&count=50&ext=mediaStats,highlightedLabel,cameraMoment");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("include_user_entities", "false"));
    par.append(O0RequestParameter("count", "100"));

    if(includeCursor){
        if(!m_cursor.isEmpty()){
            par.append(O0RequestParameter("cursor", m_cursor.toUtf8()));
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

    QVariantMap tweetObject = parseTweetV2(realTweet(m_tweets[id].toMap()), m_tweets, m_users);

//    tweetObject["user"] = m_users[tweetObject["user_id_str"].toString()];
//    //qDebug()<<tweetObject["user_id_str"].toString();
//
//    if(tweetObject["is_quote_status"].toBool() == true){
//        QString quoteId = tweetObject["quoted_status_id_str"].toString();
//
//        QVariantMap quoteTweet = parseTweet((m_tweets[quoteId].toMap()));
//
//        quoteTweet["user"] = m_users[quoteTweet["user_id_str"].toString()];
//
//        tweetObject["quoted_status"] = quoteTweet;
//
//    }

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


//    tweetModel_->clear();

    m_tweetsCount = 0;
    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().find("addEntries") != instructions[i].toMap().end()){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
            qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
//                qDebug()<<"entry: "<<i<<" of "<<entries.size();
                QStringList ids = entries[i].toMap()["entryId"].toString().split("-");

                if(ids[0].contains("tweet") && !ids[0].contains("Composer"))   // - to avoid tweetComposer on self threads
                    insertTweet(entries[i].toMap()["content"].toMap());
                else if (ids[0].contains("Thread"))
                    insertTweetsFromThread(entries[i].toMap()["content"].toMap());
                else if(ids[0].contains("cursor") && ids[1].contains("bottom"))
                    m_cursor = entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString();
                //TODO: implement cursor
            }
        }
        //TODO: implement terminate timeline
    }

    emit tweetModelChanged();
    emit conversationLoaded(m_mainTweetIndex);

    m_users.clear();
    m_tweets.clear();

    reply->deleteLater();
}
