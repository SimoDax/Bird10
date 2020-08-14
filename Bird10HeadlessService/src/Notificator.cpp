/*
 * Notificator.cpp
 *
 *  Created on: 08/ago/2020
 *      Author: Simone
 */

#include "Notificator.hpp"
#include "NotificationsApi.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/data/JsonDataAccess>

using namespace bb::platform;

Notificator::Notificator(OXTwitter* authenticator, QSettings* settings, QObject* parent) : QObject(parent)
{
    m_authenticator = authenticator;
    m_settings = settings;

}

void Notificator::checkTwitterNotifications()
{
    qDebug()<<"Service::checkTwitterNotifications";

    NotificationsApi* api = new NotificationsApi(this);

    bool ok = connect(api, SIGNAL(unreadCountChanged()), this, SLOT(onNotifications()));
    ok = connect(api, SIGNAL(networkError()), this, SLOT(onError()));

    qDebug()<<"Service::checkTwitterNotifications: CURSOR: "<<m_settings->value("notifications_cursor", QString()).toString();

    api->setAuthenticator(m_authenticator);
//    api->setCursor(m_settings.value("notifications_cursor", QString()).toString());
    api->requestNotifications();

    qDebug()<<"Service::checkTwitterNotifications API CALLED";
}

void Notificator::onNotifications()
{
    NotificationsApi* api = qobject_cast<NotificationsApi*>(sender());
    m_settings->setValue("notifications_cursor", api->cursor());

    qDebug()<<"Service::onNotificationsCount: "<< api->notifications()->size() << " UNREAD NOTIFICATIONS";

    if(api->shouldClearCache()){
        bb::data::JsonDataAccess jda;
        QVariantMap activeNotifications = jda.load("./data/activeNotifications.json").toMap();

        // clear notifications related to old entries
        for(int i = 0; i < activeNotifications.keys().size(); i++){
            if(activeNotifications.keys()[i].toULongLong() <= api->unreadIndex().toULongLong()){    //clear if the sortIndex (key) is smaller than the unread index
                QString sortIndex = activeNotifications.keys()[i];

                deleteNotification(sortIndex);
            }
        }

        // May not be needed
        // activeNotifications = jda.load("./data/activeNotifications.json").toMap();

        // now, append new entries
        for(int i = 0; i < api->notifications()->size(); i++){
            const QVariantMap& n = api->notifications()->value(i).toMap();

            if(!activeNotifications.contains(n["sortIndex"].toString())){
                sendNotification(buildBody(n) , n["sortIndex"].toString());
            }
        }
    }
    else    // just add without thinking twice
        for(int i = 0; i < api->notifications()->size(); i++){
            const QVariantMap& n = api->notifications()->value(i).toMap();

            sendNotification(buildBody(n), n["sortIndex"].toString());
        }

    api->deleteLater();
//    bb::Application::quit();
    emit done();
}

void Notificator::sendNotification(const QString& body, const QString& sortIndex)
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
    QVariantMap activeNotifications = jda.load("./data/activeNotifications.json").toMap();

    activeNotifications.insert(sortIndex, n->key());

    jda.save(activeNotifications, "./data/activeNotifications.json");

    qDebug()<<"bb::platform::NotificationError::Type: "<<n->notify();

    n->deleteLater();
}

void Notificator::deleteNotification(const QString& sortIndex)
{
    qDebug()<<"Service::deleteNotification: DELETING NOTIFICATION "<<sortIndex;

    bb::data::JsonDataAccess jda;
    QVariantMap activeNotifications = jda.load("./data/activeNotifications.json").toMap();

    QString key = activeNotifications.take(sortIndex).toString();

    Notification::deleteFromInbox(key);

    jda.save(activeNotifications, "./data/activeNotifications.json");
}

void Notificator::onError()
{
    qDebug()<<"Service::onError";
//    bb::Application::quit();
    emit done();
    sender()->deleteLater();
}

QString Notificator::buildBody(const QVariantMap& notification)
{
    if(notification["type"].toString() == "notification"){
        if(notification["tweet"].toMap()["full_text"].toString().isEmpty())
            return notification["text"].toString();

        return notification["text"].toString() + ": " + notification["tweet"].toMap()["full_text"].toString();
    }
    else if(notification["type"].toString() == "tweet"){
        return notification["user"].toMap()["screen_name"].toString() + ": " + notification["full_text"].toString();
    }
}
