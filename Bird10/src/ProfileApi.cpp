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

    appendTweets(reply, tweetModel_);   // C is back

    for(int i = 0; i < 3 && i < tweetModel_->size(); i++)
        previewTweetModel_->append(tweetModel_->value(i));

    emit previewTweetModelChanged();
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
    if (!authenticator_ || !authenticator_->linked()) {
        userModel_->clear();
        return;
    }

    QString url = ("https://api.twitter.com/graphql/hc-pka9A7gyS3xODIafnrQ/UserByScreenName");
    QList<O0RequestParameter> par;
    QString query("{\"screen_name\":\""+ m_screen_name +"\",\"withHighlightedLabel\":true}");
    par.append(O0RequestParameter("variables", query.toUtf8()));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onUserData()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ProfileApi::onUserData(){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    QVariantMap userData = content["data"].toMap()["user"].toMap()["legacy"].toMap();
    userData["id_str"] = content["data"].toMap()["user"].toMap()["rest_id"].toString();

    std::wstring desc = userData["description"].toString().toStdWString();
    rewriteUrls(desc, userData["entities"].toMap()["description"].toMap()["urls"].toList());
    userData["description"] = QString::fromWCharArray(desc.c_str());

    populateUserData(userData);
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
                    QVariantMap tweetObject = parseTweetV2((tweets[id].toMap()), tweets, users);

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


void ProfileApi::populateUserData(const QVariantMap& data)
{
    userModel_->clear();
    userModel_->append(data);
    userModel_->append(data);

    m_id_str = data["id_str"].toString();
    //qDebug()<<m_id_str;
    m_following = data["following"].toBool();

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
