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

#include "DMNotificator.hpp"
#include "DMApi.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/data/JsonDataAccess>

using namespace bb::platform;

DMNotificator::DMNotificator(OXTwitter* authenticator, QSettings* settings, QObject* parent) : Notificator(authenticator, settings, parent)
{
    // TODO Auto-generated constructor stub

}

void DMNotificator::checkTwitterNotifications()
{
    qDebug()<<"Service::checkTwitterNotifications";

    DMApi* api = new DMApi(this);
    api->setAuthenticator(m_authenticator);

    bool ok = connect(api, SIGNAL(unreadCountChanged()), this, SLOT(onNotifications()));
    ok = connect(api, SIGNAL(networkError()), this, SLOT(onError()));

    QString cursor = m_settings->value("dm_cursor").toString();
    if(cursor.isEmpty()){
        qDebug()<<"Service::checkTwitterNotifications: EMPTY CURSOR";
        api->loadInboxInitialState();
    }
    else{
        qDebug()<<"Service::checkTwitterNotifications: VALID CURSOR";
        api->setCursor(cursor);
        api->pollUpdates();
    }
}

void DMNotificator::onNotifications()
{
    DMApi* api = qobject_cast<DMApi*>(sender());

    qDebug()<<"Service::DMNotificator::onNotifications: "<<api->conversations().size()<<" *NEW* UNREAD DM";

    if(api->lastSeenId() != m_settings->value("dm_last_seen_event_id").toULongLong()){   // user has opened DMs tab or it's the first time

        deleteAll();    // since last_seen_event_id is updated the user has seen all past dm updates

        // add any new notifications
        for(int i = 0; i < api->conversations().size(); i++){
            const QVariantMap& c = api->conversations().value(i).toMap();

            sendNotification(buildBody(c) , c["conversation_id"].toString());
        }
    }
    else{   // nothing much... add any new conversation and refresh current ones when new messages arrived
            // Since the updates poll is cursor-based all the stuff happened since last request will be returned by the api

        bb::data::JsonDataAccess jda;
        QVariantMap activeNotifications = jda.load("./data/activeDMs.json").toMap();

        for(int i = 0; i < api->conversations().size(); i++){
            const QVariantMap& c = api->conversations().value(i).toMap();

            if(activeNotifications.keys().contains(c["conversation_id"].toString()))
            {

                // notification for current conversation already exixts
                deleteNotification(c["conversation_id"].toString());
                sendNotification(buildBody(c) , c["conversation_id"].toString());
            }
            else
                sendNotification(buildBody(c) , c["conversation_id"].toString());
        }
    }

    m_settings->setValue("dm_cursor", api->cursor());
    m_settings->setValue("dm_last_seen_event_id", api->lastSeenId());
    api->deleteLater();

    emit done();
}

QString DMNotificator::buildBody(const QVariantMap& conversation)
{
    if(conversation["screen_name"].toString() != "")    // if not a group chat
        return conversation["name"].toString() + " (@" + conversation["screen_name"].toString() + ") sent you a DM";
    else
        return "New message in " + conversation["name"].toString();
}

void DMNotificator::sendNotification(const QString& body, const QString& conversation_id)
{
    qDebug()<<"Service::sendNotification: "<<body;

    bb::platform::Notification* n = new Notification(this);

    n->setTitle(NOTIFICATION_TITLE);
    n->setBody(body);

    bb::system::InvokeRequest request;
    request.setTarget("com.simodax.bird10");
    request.setAction("bb.action.START");
    n->setInvokeRequest(request);

    bb::data::JsonDataAccess jda;
    QVariantMap activeNotifications = jda.load("./data/activeDMs.json").toMap();

    activeNotifications.insert(conversation_id, n->key());

    jda.save(activeNotifications, "./data/activeDMs.json");

    qDebug()<<"bb::platform::NotificationError::Type: "<<n->notify();

    n->deleteLater();
}

void DMNotificator::deleteNotification(const QString& conversation_id)
{
    qDebug()<<"Service::deleteNotification: DELETING NOTIFICATION "<<conversation_id;

    bb::data::JsonDataAccess jda;
    QVariantMap activeNotifications = jda.load("./data/activeDMs.json").toMap();

    QString key = activeNotifications.take(conversation_id).toString();

    Notification::deleteFromInbox(key);

    jda.save(activeNotifications, "./data/activeDMs.json");
}

void DMNotificator::deleteAll()
{
    bb::data::JsonDataAccess jda;
    QVariantMap activeNotifications = jda.load("./data/activeDMs.json").toMap();

    for(QMap<QString, QVariant>::iterator i = activeNotifications.begin(); i != activeNotifications.end(); i++)
        Notification::deleteFromInbox(i.value().toString());

    QFile::remove("./data/activeDMs.json");
}
