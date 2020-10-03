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

#include <src/ProfileApi.hpp>
#include <src/QJson4/QJsonArray.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonValue.h>
#include <src/QJson4/QJsonObject.h>


ProfileApi::ProfileApi(QObject *parent) : TimelineBase(parent)
{
    previewTweetModel_ = new TimelineDataModel(this);
    tweetModel_ = new TimelineDataModel(this);
    mediaTweetModel_ = new TimelineDataModel(this);
    fullTweetModel_ = new TimelineDataModel(this);
    userModel_ = new TimelineDataModel(this);

}

void ProfileApi::requestUserTweets(QString max_id, QString since_id)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/statuses/user_timeline.json?tweet_mode=extended&count=100&exclude_replies=true");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "100"));
    par.append(O0RequestParameter("exclude_replies", "true"));

    if(!m_screen_name.isEmpty()){
        url += "&screen_name=" + m_screen_name;
        par.append(O0RequestParameter("screen_name", m_screen_name.toUtf8()));
    }
    if(!max_id.isEmpty()){
        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
    }
    if(!since_id.isEmpty()){
        url += "&since_id=" + QString("%1").arg(since_id.toLongLong()-1);
        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()-1).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(!max_id.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onOlderTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onTweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ProfileApi::requestOlderUserTweets()
{
    QString last_tweet_id = tweetModel_->value(tweetModel_->size()-1-1).toMap()["id_str"].toString();

    requestUserTweets(last_tweet_id);
}


void ProfileApi::onTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonArray jsonArray = QJsonDocument::fromJson(reply->data()).array();

    qDebug()<<"ProfileApi::tweetsReceived";

    tweetModel_->clear();
    previewTweetModel_->clear();
    userModel_->clear();

    appendTweets(reply, tweetModel_);   // C is back

    for(int i = 0; i < 3 && i < tweetModel_->size(); i++)
        previewTweetModel_->append(tweetModel_->value(i));

    if(!jsonArray.isEmpty())        // if profile is not private we can get user info from their tweets
        populateUserData(jsonArray);
    else
        requestUserData();

    reply->deleteLater();
}

void ProfileApi::onOlderTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    tweetModel_->removeAt(tweetModel_->size()-1);       // Remove loading element
    appendTweets(reply, tweetModel_);

    reply->deleteLater();
}

void ProfileApi::requestUserData()
{
}


void ProfileApi::appendTweets(CurlEasy* reply, TimelineDataModel* dataModel)
{
    // V1.1 timeline parser
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonArray jsonArray = jsonResponse.array();
    if(!jsonArray.isEmpty()){
        foreach (const QJsonValue &v, jsonArray) {
            QVariantMap item = v.toObject().toVariantMap();
            QVariantMap tweet = realTweet(item);

            dataModel->append(parseTweet(tweet));
        }
        dataModel->append(QVariant());    //Empty QVariant for loading element

        emit tweetModelChanged();
    }

}

void ProfileApi::requestFullUserTweets(QString max_id, QString since_id)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/statuses/user_timeline.json?tweet_mode=extended&count=100");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "100"));

    if(!m_screen_name.isEmpty()){
        url += "&screen_name=" + m_screen_name;
        par.append(O0RequestParameter("screen_name", m_screen_name.toUtf8()));
    }
    if(!max_id.isEmpty()){
        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
    }
    if(!since_id.isEmpty()){
        url += "&since_id=" + QString("%1").arg(since_id.toLongLong()-1);
        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()-1).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(!max_id.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onOlderFullTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onFullTweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ProfileApi::requestOlderFullUserTweets()
{
    QString last_tweet_id = fullTweetModel_->value(fullTweetModel_->size()-1-1).toMap()["id_str"].toString();

    requestFullUserTweets(last_tweet_id);
}

void ProfileApi::requestMediaUserTweets(QString cursor)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/timeline/media/" + m_id_str + ".json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("include_user_entities", "true"));
    par.append(O0RequestParameter("count", "20"));

    if(!cursor.isEmpty()){
        par.append(O0RequestParameter("cursor", cursor.toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(!cursor.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onOlderMediaTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onMediaTweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ProfileApi::requestOlderMediaUserTweets()
{
    requestMediaUserTweets(m_mediaCursor);
}

void ProfileApi::onFullTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonArray jsonArray = QJsonDocument::fromJson(reply->data()).array();

    qDebug()<<"ProfileApi::onFullTweetsReceived";

    fullTweetModel_->clear();
    appendTweets(reply, fullTweetModel_);

    reply->deleteLater();

}

void ProfileApi::onOlderFullTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    fullTweetModel_->removeAt(fullTweetModel_->size()-1);       // Remove loading element
    appendTweets(reply, fullTweetModel_);

    reply->deleteLater();
}

void ProfileApi::onMediaTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    mediaTweetModel_->clear();
    insertMediaTweets(reply);

    reply->deleteLater();
}

void ProfileApi::insertMediaTweets(CurlEasy* reply, int offset)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    QVariantMap users = content["globalObjects"].toMap()["users"].toMap();
    QVariantMap tweets = content["globalObjects"].toMap()["tweets"].toMap();

    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();

    int cont=0;
    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().find("addEntries") != instructions[i].toMap().end()){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
            //qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
                QStringList ids = entries[i].toMap()["entryId"].toString().split("-");

                if(ids[0].contains("tweet")){
                    // Append tweet item

                    QVariantMap tweet = entries[i].toMap()["content"].toMap();
                    QString id = tweet["item"].toMap()["content"].toMap()["tweet"].toMap()["id"].toString();
                    QVariantMap tweetObject = parseTweetV2(realTweet(tweets[id].toMap()), tweets, users);

                    if(tweetObject.keys().contains("extended_entities")){   // Filter links

//                        tweetObject["user"] = users[tweetObject["user_id_str"].toString()];

                        // For each media item append a new object to the gallery
                        QVariantList mediaList = tweetObject["extended_entities"].toMap()["media"].toList();
                        for(int i = 0; i < mediaList.size(); i++){

                            QVariantMap mediaItem = mediaList[i].toMap();
                            QVariantList media;
                            media.append(mediaItem);
                            QVariantMap extended_entities;
                            extended_entities["media"] = media;

                            tweetObject["extended_entities"] = extended_entities;

                            //qDebug()<<tweetObject["user_id_str"].toString();
                            // Follow workaround to make ListView stay on the same item when updating the model
                            if(cont + offset == mediaTweetModel_->size()-1)             //if last element then it's a loading element, replace it with the new image
                                mediaTweetModel_->replace(offset+cont,tweetObject);
                            else
                                mediaTweetModel_->insert(offset+cont,tweetObject);      // otherwise append it normally
                            cont++;
                        }
                    }
                }
                else if (ids[0] == "cursor" && ids[1] == "bottom"){
                    m_mediaCursor = entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString();
                }
            }
            if(cont)
                mediaTweetModel_->append(QVariant());                                   // add loading element
            else
                mediaTweetModel_->removeAt(mediaTweetModel_->size()-1);                 // remove loading element
        }
        //TODO: implement terminate timeline
    }

}

void ProfileApi::onOlderMediaTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    insertMediaTweets(reply, mediaTweetModel_->size()-1);

    reply->deleteLater();
}


void ProfileApi::populateUserData(const QJsonArray& tweets)
{
    userModel_->clear();
    userModel_->append(tweets[0].toObject().toVariantMap()["user"].toMap());
    userModel_->append(tweets[0].toObject().toVariantMap()["user"].toMap());

    m_id_str = tweets[0].toObject().toVariantMap()["user"].toMap()["id_str"].toString();
    //qDebug()<<m_id_str;
    m_following = tweets[0].toObject().toVariantMap()["user"].toMap()["following"].toBool();

    emit followingChanged();
    emit userModelChanged();
}

void ProfileApi::followUser(const QString& id, bool following)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url;
    if(following)
        url = ("https://api.twitter.com/1.1/friendships/destroy.json");
    else
        url = ("https://api.twitter.com/1.1/friendships/create.json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("screen_name", id.toUtf8()));

    CurlEasy* reply = requestor->post(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onFollowed()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ProfileApi::onFollowed(){
    m_following = ! m_following;

    if(m_following)
        emit followed();
    else
        emit unfollowed();

    emit followingChanged();
}

//QVariantMap ProfileApi::realTweet(QVariantMap tweet)
//{
//    if(tweet.keys().contains("retweeted_status")){
//        QVariantMap real_tweet = tweet["retweeted_status"].toMap();
//        //real_tweet["id_str"] = tweet["id_str"];     //omg haxor
//        real_tweet["rt_flag"] = true;
//        real_tweet["rt_name"] = tweet["user"].toMap()["name"];
//        real_tweet["rt_screen_name"] = tweet["user"].toMap()["screen_name"];
//        real_tweet["rt_id"] = tweet["id_str"];
//        return real_tweet;
//    }
//    else{
//        tweet["rt_flag"] = false;
//        return tweet;
//    }
//}
//
//QVariantMap ProfileApi::parseTweet(QVariantMap tweet)
//{
//    // Parse media type (if any)
//
//    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "photo")
//        tweet["image_flag"] = true;
//    else
//        tweet["image_flag"] = false;
//
//    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "video")
//        tweet["video_flag"] = true;
//    else
//        tweet["video_flag"] = false;
//
//    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "animated_gif")
//        tweet["gif_flag"] = true;
//    else
//        tweet["gif_flag"] = false;
//
//
//    // Rewrite tweet, use utf-32 to count emoji only once
//    std::wstring text = tweet["full_text"].toString().toStdWString();
//
//    if(tweet.keys().contains("display_text_range")){
//        // Crop quoted tweet link
//        text.erase(tweet["display_text_range"].toList()[1].toInt());
//    }
//
//    QVariantList urls = tweet["entities"].toMap()["urls"].toList();
//
//    int offset = 0;
//    for(int i = 0; i<urls.size(); i++){
//        int beginIndex = urls[i].toMap()["indices"].toList()[0].toInt();
//        if(beginIndex+offset >= text.size())   //link already cropped out by display_text_range
//            continue;
//        int endIndex = urls[i].toMap()["indices"].toList()[1].toInt();
//
//        //QString htmlUrl = "<a href = \"" + urls[i].toMap()["url"].toString() + "\">"+ urls[i].toMap()["expanded_url"].toString() + "</a>";
//        QString htmlUrl = urls[i].toMap()["expanded_url"].toString();   // coupled with content.flags in textbox
//
//        text.replace(beginIndex+offset, endIndex-beginIndex, htmlUrl.toStdWString());
//        //text.remove(beginIndex+offset, endIndex-beginIndex);
//        //text.insert(beginIndex+offset, htmlUrl);
//
//        offset += htmlUrl.length() - (endIndex - beginIndex);   //keeps track of indexes changes due to html insertion
//
//    }
//    tweet["full_text"] = QString::fromStdWString(text);
//
//    return tweet;
//}

