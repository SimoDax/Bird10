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

#include <src/NotificationsApi.hpp>
#include <QObject>
#include <src/QJson4/QJsonObject.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/TwitterApiBase.hpp>

#include <bb/data/JsonDataAccess.hpp>
using namespace bb::data;

NotificationsApi::NotificationsApi(QObject *parent) : TwitterApiBase(parent)
{
    m_notificationModel = new QVariantList();
    m_shouldClearCache = false;
}

void NotificationsApi::requestNotifications(){
    if (!authenticator_ || !authenticator_->linked()) {
        qDebug()<<"Service::NotificationsApi::requestNotifications: BAD AUTHENTICATOR!";
        m_notificationModel->clear();
        emit notificationModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/notifications/all.json?include_profile_interstitial_type=1&include_blocking=1&include_blocked_by=1&include_followed_by=1&include_want_retweets=1&include_mute_edge=1&include_can_dm=1&include_can_media_tag=1&skip_status=1&cards_platform=Web-12&include_cards=1&include_composer_source=true&include_ext_alt_text=true&include_reply_count=1&tweet_mode=extended&include_entities=true&include_user_entities=true&include_ext_media_color=false&include_ext_media_availability=false&send_error_codes=true&simple_quoted_tweets=true&count=40&ext=mediaStats,highlightedLabel,cameraMoment");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("count", "10"));
    par.append(O0RequestParameter("include_user_entities", "false"));

    if(!m_cursor.isEmpty()){
        url += "&cursor=" + m_cursor;
        par.append(O0RequestParameter("cursor", m_cursor.toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onNotificationsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

    qDebug()<<"Service::NotificationsApi::requestNotifications: PERFORM INVOKED";
}

void NotificationsApi::onNotificationsReceived(){

    qDebug()<<"Service::NotificationsApi::onNotificationsReceived";

    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    qDebug()<<reply->data();

    JsonDataAccess jda;
    QVariant c;

    try{
        c = jda.loadFromBuffer(reply->data());
    }
    catch(const std::bad_alloc &){
        qDebug()<<"BAD ALLOC!";
    }
    catch(...){
        qDebug() << "Handling exception not caught in slot.";
    }
    QVariantMap content = c.toMap();

//    reply->deleteLater();   // from now on buffer containing data() is emptied, to save memory


    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();

    m_notifications = content["globalObjects"].toMap()["notifications"].toMap();
    m_tweets = content["globalObjects"].toMap()["tweets"].toMap();
    m_users = content["globalObjects"].toMap()["users"].toMap();

    for(int i = 0; i < instructions.size(); i++){
        if (instructions[i].toMap().keys().contains("markEntriesUnreadGreaterThanSortIndex"))
                m_unreadIndex = instructions[i].toMap()["markEntriesUnreadGreaterThanSortIndex"].toMap()["sortIndex"].toString();
    }

    for(int i = 0; i < instructions.size(); i++){
//        qDebug()<<instructions[i];
        if(instructions[i].toMap().keys().contains("addEntries")){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();

            int count = 0;      // Always append from the top of the list
            for(int i = 0; i<entries.size(); i++){

                QStringList ids = entries[i].toMap()["entryId"].toString().split("-");

                if(ids[0].contains("notification") && entries[i].toMap()["sortIndex"].toString() > m_unreadIndex){  // only append unread items
                    m_unreadCount++;

                    QVariantMap content = entries[i].toMap()["content"].toMap()["item"].toMap()["content"].toMap();
                    if(content.find("notification") != content.end())
                        count = insertNotification(content["notification"].toMap()["id"].toString(), count, entries[i].toMap()["sortIndex"].toString());
                    else if (content.find("tweet") != content.end())
                        count = insertTweet(content["tweet"].toMap()["id"].toString(), count, entries[i].toMap()["sortIndex"].toString());
                }
                if(ids[0].contains("cursor") && ids[1].contains("top"))
                    m_cursor = entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString();
            }
        }
        else if(instructions[i].toMap().keys().contains("clearCache")){
            m_shouldClearCache = true;
        }
    }

    emit notificationModelChanged();

    qDebug()<<"Service::Unread index: "<<m_unreadIndex;
    m_unreadCount = 0;
    for(int i = 0; i < m_notificationModel->size(); i++){
         if(m_notificationModel->value(i).toMap()["sortIndex"].toString() > m_unreadIndex)
             m_unreadCount++;

    }

    m_notifications.clear();
    m_tweets.clear();
    m_users.clear();
    reply->deleteLater();

    emit unreadCountChanged();
}

void NotificationsApi::updateUnreadIndex(){
    if (!authenticator_ || !authenticator_->linked()) {
        m_notificationModel->clear();
        emit notificationModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/notifications/all/last_seen_cursor.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("cursor", m_cursor.toUtf8()));

    CurlEasy* reply = requestor->post(url, par, QByteArray());

    connect(reply, SIGNAL(done(CURLcode)), reply, SLOT(deleteLater()));     // no content returned
    connect(reply, SIGNAL(error(CURLcode)), reply, SLOT(deleteLater()));     // can't do anything if fails

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

    m_unreadCount = 0;
    emit unreadCountChanged();
}

int NotificationsApi::insertNotification(const QString& id, int pos, const QString& sortIndex){
    QVariantMap n = m_notifications[id].toMap();

    QVariantMap item;
    item["type"] = "notification";
    item["text"] = n["message"].toMap()["text"].toString();

    QVariantList targetObjects = n["template"].toMap()["aggregateUserActionsV1"].toMap()["targetObjects"].toList();
    if(targetObjects.size()){
        QVariantMap tweet = qvariant_cast<QVariantMap>(m_tweets[targetObjects[0].toMap()["tweet"].toMap()["id"].toString()]);
//        tweet["user"] = m_users[tweet["user_id_str"].toString()];

        item["tweet"] = tweet;

        item["tweet_flag"] = true;
    }
    else item["tweet_flag"] = false;

    QVariantList fromUsers = n["template"].toMap()["aggregateUserActionsV1"].toMap()["fromUsers"].toList();
    if(fromUsers.size()){
        QVariantMap user = m_users[fromUsers[0].toMap()["user"].toMap()["id"].toString()].toMap();
        item["profile_image_url_https"] = user["profile_image_url_https"];
        item["user_flag"] = true;
    }
    else item["user_flag"] = false;

    item["sortIndex"] = sortIndex;

    m_notificationModel->insert(pos, item);
    return pos+1;
}

int NotificationsApi::insertTweet(const QString& id, int pos, const QString& sortIndex){
    QVariantMap item = m_tweets[id].toMap();
    item["user"] = m_users[item["user_id_str"].toString()];

    item["type"] = "tweet";
    item["sortIndex"] = sortIndex;
    m_notificationModel->insert(pos, item);


    return pos+1;
}

void NotificationsApi::clearNotifications(){

    m_notificationModel->clear();
    m_unreadCount = 0;
    m_unreadIndex.clear();
    m_cursor.clear();

    emit notificationModelChanged();
}
