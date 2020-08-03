/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>
#include <bb/data/JsonDataAccess>

#include <QTimer>
#include "src/NotificationsApi.hpp"
#include <curl/curl.h>

#ifdef QT_DEBUG
#define NOTIFICATION_TITLE "Bird10 (debug)"
#else
#define NOTIFICATION_TITLE "Bird10"
#endif

using namespace bb::platform;
using namespace bb::system;

QThread Service::curlThread;

Service::Service() : QObject(),
        m_notify(new Notification(this)),
        m_invokeManager(new InvokeManager(this)),
        m_authenticator(new OXTwitter(this)),
        m_timer(new QTimer(this)),
        m_settings("simodax", "Bird10HeadlessService")
{
    qRegisterMetaType<CURLcode>("CURLcode");
    curlThread.start();

    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));


    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();

//    qDebug()<<"SERVICE TOKEN: "<<m_authenticator->token();

//    onTimeout();
}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    qDebug()<<"Service::handleInvoke: ACTION IS "<<request.action();

    // Check if we can hit twitter
    if(!m_authenticator->linked()){
        // deregister and close
        clearNotifications();
        clearTimer();
    }

    // Ok, we're logged in - carry on
    if (request.action().compare("com.simodax.Bird10HeadlessService.RESET") == 0) {
        qDebug()<<"SERVICE TOKEN (on reset): "<<m_authenticator->token();

        // Register timer and check for notifications
        registerTimer();
        checkTwitterNotifications();

    }
    else if(request.action().compare("bb.action.system.TIMER_FIRED") == 0){

        // Sometimes the STOP action happens just before the timer fires and the invocation platform can't deregister it quickly enough
        // so we should check if the sevice is supposed to be in active state before scheduling the next execution
        if(m_settings.value("active", false).toBool() == false){
            qDebug()<<"Service::handleInvoke: QUITTING BECAUSE active = false !";
            bb::Application::quit();
        }

        // Register timer and check for notifications
        registerTimer();
        checkTwitterNotifications();
    }
    else if(request.action().compare("com.simodax.Bird10HeadlessService.STOP") == 0){
        // deregister and close
        clearTimer();
    }

}


void Service::onTimeout()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();
    m_notify->notify();
}

void Service::registerTimer()
{
    qDebug()<<"Service::registerTimer: SETTING TIMER";

    // Sometimes the STOP action happens just before the timer fires and the invocation platform can't deregister it quickly enough
    // so we should check if the sevice is supposed to be in active state before scheduling the next execution
//    if(m_settings.value("started", false).toBool())  // don't register timer if it already exists
//        return;

//    connect(m_timer, SIGNAL(timeout()), this, SLOT(checkTwitterNotifications()));
//    m_timer->start(60000);


    // Set the timer trigger for 1 minute from now
    InvokeDateTime trigdatetime(QDate::currentDate(), QTime::currentTime().addSecs(60), "");

    // Create the timer request
    InvokeTimerRequest timer_request("com.simodax.bird10.TIMER", trigdatetime, "com.simodax.Bird10HeadlessService");

    m_invokeReply = m_invokeManager->registerTimer(timer_request);
    bool ok = connect(m_invokeReply, SIGNAL(finished()), this, SLOT(onInvokeResult()));

//
//    InvokeRecurrenceRule recurrenceRule(bb::system::InvokeRecurrenceRuleFrequency::Minutely);
//    recurrenceRule.setInterval(1);
//
//    InvokeTimerRequest timer_request;
//
//    if (recurrenceRule.isValid()) {
//        timer_request.set("com.simodax.bird10.TIMER",  recurrenceRule, "com.simodax.Bird10HeadlessService");
//        m_invokeReply = m_invokeManager->registerTimer(timer_request);
//
//        bool ok = connect(m_invokeReply, SIGNAL(finished()), this, SLOT(onInvokeResult()));
//
////        checkTwitterNotifications();
//    }

    qDebug()<<"Service::start: TIMER SET";
}

void Service::clearTimer()
{
    qDebug()<<"Service::stop";

    m_invokeManager->deregisterTimer("com.simodax.bird10.TIMER");
    m_settings.setValue("active", false);
    bb::Application::quit();
}

void Service::clearNotifications()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();
}

void Service::checkTwitterNotifications()
{
    qDebug()<<"Service::checkTwitterNotifications";

    NotificationsApi* api = new NotificationsApi(this);

    bool ok = connect(api, SIGNAL(unreadCountChanged()), this, SLOT(onNotificationsCount()));
    ok = connect(api, SIGNAL(networkError()), this, SLOT(onError()));

    qDebug()<<"Service::checkTwitterNotifications: CURSOR: "<<m_settings.value("notifications_cursor", QString()).toString();

    api->setAuthenticator(m_authenticator);
//    api->setCursor(m_settings.value("notifications_cursor", QString()).toString());
    api->requestNotifications();

    qDebug()<<"Service::checkTwitterNotifications API CALLED";
}

void Service::onNotificationsCount()
{
    NotificationsApi* api = qobject_cast<NotificationsApi*>(sender());
    m_settings.setValue("notifications_cursor", api->cursor());

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
    bb::Application::quit();
}

void Service::deleteNotification(const QString& sortIndex)
{
    qDebug()<<"Service::deleteNotification: DELETING NOTIFICATION "<<sortIndex;

    bb::data::JsonDataAccess jda;
    QVariantMap activeNotifications = jda.load("./data/activeNotifications.json").toMap();

    QString key = activeNotifications.take(sortIndex).toString();

    Notification::deleteFromInbox(key);

    jda.save(activeNotifications, "./data/activeNotifications.json");
}

void Service::sendNotification(const QString& body, const QString& sortIndex)
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

QString Service::buildBody(const QVariantMap& notification)
{
    if(notification["tweet"].toMap()["full_text"].toString().isEmpty())
        return notification["text"].toString();

    return notification["text"].toString() + ": " + notification["tweet"].toMap()["full_text"].toString();
}


void Service::onInvokeResult(){   //helper function pasted stright from docs, saves a lot of headaches
    // Check for errors
    switch(m_invokeReply->error()) {
        case InvokeReplyError::None: {
            qDebug() << "Service::invokeFinished(): No error" << endl;
            m_settings.setValue("active", true);
            break;
        }
        // Invocation could not find the target;
        // did we use the right target ID?
        case InvokeReplyError::NoTarget: {
            qDebug() << "Service::invokeFinished(): Error: no target" << endl;
            m_settings.setValue("active", false);
            break;
        }

        // There was a problem with the invocation request;
        // did we set all of the values correctly?
        case InvokeReplyError::BadRequest: {
            qDebug() << "Service::invokeFinished(): Error: bad request" << endl;
            m_settings.setValue("active", false);
            break;
        }

        // Something went completely
        // wrong inside the invocation request,
        // so find an alternate route
        case InvokeReplyError::Internal: {
            qDebug() << "Service::invokeFinished(): Error: internal" << endl;
            m_settings.setValue("active", false);
            break;
        }

        // Message received if the invocation request is successful
        default:
            qDebug() << "Service::invokeFinished(): " << m_invokeReply->error() <<endl;
            m_settings.setValue("active", false);
            break;
    }
    m_invokeReply->deleteLater();
}

void Service::onError()
{
    qDebug()<<"Service::onError";
    bb::Application::quit();
}
