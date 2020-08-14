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

#include <src/DMApi.hpp>
#include <src/QJson4/QJsonObject.h>
#include <src/QJson4/QJsonDocument.h>
#include <iostream>

#include <bb/data/JsonDataAccess.hpp>
using namespace bb::data;

bool lessThan(const QVariant& l, const QVariant& r);

DMApi::DMApi(QObject *parent) : TwitterApiBase(parent)
{
    m_polling = 0;
}

DMApi::~DMApi()
{
}

void DMApi::loadInboxInitialState()
{
    if (!authenticator_ || !authenticator_->linked()) {
            emit inboxModelChanged();
            return;
    }

    QString url = ("https://api.twitter.com/1.1/dm/inbox_initial_state.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("include_groups", "true"));
    par.append(O0RequestParameter("include_inbox_timelines", "true"));
    par.append(O0RequestParameter("supports_reactions", "false"));
    par.append(O0RequestParameter("filter_low_quality", "true"));
    par.append(O0RequestParameter("include_quality", "all"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onInboxInitialStateReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DMApi::onInboxInitialStateReceived(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

//    saveReply(reply, "dm");

    QByteArray text = reply->data();

    int beginIndex = text.indexOf("{\"inbox_initial_state\":{\"last_seen_event_id\":\"");
    if(beginIndex != -1){
        int endIndex = text.indexOf("\"", 46);
//        qDebug()<<"Service::DMApi::onInboxInitialStateReceived: "<<text.mid(46, endIndex-46);
        m_lastSeenId = text.mid(46, endIndex-46).toULongLong();

        qDebug()<<"Service::DMApi::onInboxInitialStateReceived: LAST SEEN ID "<<m_lastSeenId;

        beginIndex = text.indexOf("\"cursor\":\"") +10;
        endIndex = text.indexOf("\"", beginIndex);
        m_cursor = text.mid(beginIndex, endIndex-beginIndex);
        qDebug()<<"Service::DMApi::onInboxInitialStateReceived: CURSOR "<<m_cursor;


        beginIndex = text.indexOf("],\"users\":{");
        if(beginIndex != -1){
            text.remove(0, beginIndex+2);
            text.prepend("{");
            text.chop(1);

//            QFile file("./data/text.txt");
//            file.open(QIODevice::WriteOnly);
//            if(file.isOpen()){
//                QTextStream out(&file);
//                out << text;
//            }
//            file.close();

            JsonDataAccess jda;
            QVariantMap content = jda.loadFromBuffer(text).toMap();

            insertConversations(content);
        }
    }

    emit unreadCountChanged();
    reply->deleteLater();

}



void DMApi::pollUpdates(const QString& conversationId)
{
    if (!authenticator_ || !authenticator_->linked()) {
            emit inboxModelChanged();
            return;
    }

//    if(m_polling)
//        return;
//
//    m_polling = true;

    QString url = ("https://api.twitter.com/1.1/dm/user_updates.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("cursor", m_cursor.toUtf8()));
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("include_groups", "true"));
    par.append(O0RequestParameter("supports_reactions", "false"));

    if(!conversationId.isEmpty())
        par.append(O0RequestParameter("active_conversation_id", conversationId.toUtf8()));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onUpdates()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onUpdatesFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void DMApi::onUpdates(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QByteArray text = reply->data();

//    qDebug()<<"Service::DMApi::onUpdates: "<<reply->data();

    int beginIndex = text.indexOf("\"cursor\":\"") + 10;
    if(beginIndex != -1){
        int endIndex = text.indexOf("\"", beginIndex);
        m_cursor = text.mid(beginIndex, endIndex-beginIndex);

        qDebug()<<"Service::DMApi::onUpdates: CURSOR "<<m_cursor;

        beginIndex = text.indexOf("\"last_seen_event_id\":\"") +22;
        endIndex = text.indexOf("\"", beginIndex);
        m_lastSeenId = text.mid(beginIndex, endIndex-beginIndex).toULongLong();
        qDebug()<<"Service::DMApi::onUpdates: LAST SEEN ID "<<m_lastSeenId;


        beginIndex = text.indexOf("],\"users\":{");
        if(beginIndex != -1){
            text.remove(0, beginIndex+2);
            text.prepend("{");
            text.chop(1);

            JsonDataAccess jda;
            QVariantMap content = jda.loadFromBuffer(text).toMap();

            insertConversations(content);
        }
    }

    emit unreadCountChanged();
    reply->deleteLater();
}

void DMApi::onUpdatesFailed(CURLcode c){
    onRequestFailed(c);

    m_polling = false;
}


void DMApi::insertConversations(const QVariantMap& content)
{
    foreach(QVariant conversation, content["conversations"].toMap().values()){
        QVariantMap c = conversation.toMap();

        if(c["sort_event_id"].toULongLong() > m_lastSeenId){
            // parse message with user..
            QString conv_id = c["conversation_id"].toString();

            if(conv_id.contains("-")){
                // it's a one-to-one chat
                // find who is on the other end, let's try with the sender

                QString other_id = c["participants"].toList()[0].toMap()["user_id"].toString();
                if(other_id == authenticator_->extraTokens()["user_id"]){
                    // nope, the sender is me, the other person is the recipient
                    other_id = c["participants"].toList().value(1).toMap()["user_id"].toString();
                }

                QVariantMap user = content["users"].toMap()[other_id].toMap();
                c["name"] = user["name"];
                c["screen_name"] = user["screen_name"];
            }
            else{
                // it's a group chat

                c["screen_name"] = "";
            }

            m_conversations.append(c);
            m_unreadCount++;
        }
    }
}


