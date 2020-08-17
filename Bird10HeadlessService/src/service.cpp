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
#include "src/DMApi.hpp"
#include "src/Notificator.hpp"
#include "src/DMNotificator.hpp"
#include <curl/curl.h>


using namespace bb::platform;
using namespace bb::system;

QThread Service::curlThread;

Service::Service() : QObject(),
        m_invokeManager(new InvokeManager(this)),
        m_authenticator(new OXTwitter(this)),
        m_settings(new QSettings("simodax", "Bird10HeadlessService")),
        m_running(false)
{
    qRegisterMetaType<CURLcode>("CURLcode");
    curlThread.start();

    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));


//    NotificationDefaultApplicationSettings settings;
//    settings.setPreview(NotificationPriorityPolicy::Allow);
//    settings.apply();

//    bb::platform::Notification* n = new Notification(this);
//
//    n->setTitle(NOTIFICATION_TITLE);
//    n->setBody("Service started");
//
//    n->notify();

//    n->deleteLater();

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

        bb::platform::Notification* n = new Notification(this);

        n->setTitle(NOTIFICATION_TITLE);
        n->setBody("Service started");
        n->notify();
        n->deleteLater();

        // Register timer and check for notifications
        registerTimer();
        checkNotifications();

    }
    else if(request.action().compare("bb.action.system.TIMER_FIRED") == 0){

        // Sometimes the STOP action happens just before the timer fires and the invocation platform can't deregister it quickly enough
        // so we should check if the sevice is supposed to be in active state before scheduling the next execution
        if(m_settings->value("active", false).toBool() == false){
            qDebug()<<"Service::handleInvoke: QUITTING BECAUSE active = false !";
            bb::Application::quit();
            return;
        }

        // Register timer and check for notifications
        registerTimer();
        checkNotifications();
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

    qDebug()<<"Service::start: TIMER SET";
}

void Service::clearTimer()
{
    qDebug()<<"Service::stop";

    m_invokeManager->deregisterTimer("com.simodax.bird10.TIMER");
    m_settings->setValue("active", false);
    bb::Application::instance()->requestExit();
//    bb::Application::quit();
}


void Service::clearNotifications()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();
}


void Service::checkNotifications()
{
    if(m_running)   // if the service gets called while the previous instance is still running let it finish
        return;

    m_running = true;

    Notificator* n = new Notificator(m_authenticator, m_settings, this);
    connect(n, SIGNAL(done()), this, SLOT(checkDM()));
    n->checkTwitterNotifications();
}

void Service::checkDM()
{
    if(sender())
        sender()->deleteLater();
//        delete sender();

    qDebug()<<"Service::checkDM";

    DMNotificator* n = new DMNotificator(m_authenticator, m_settings, this);

    bool ok = connect(n, SIGNAL(done()), this, SLOT(quit()));

    n->checkTwitterNotifications();
}

void Service::onInvokeResult(){   //helper function pasted stright from docs, saves a lot of headaches
    // Check for errors
    switch(m_invokeReply->error()) {
        case InvokeReplyError::None: {
            qDebug() << "Service::invokeFinished(): No error" << endl;
            m_settings->setValue("active", true);
            break;
        }
        // Invocation could not find the target;
        // did we use the right target ID?
        case InvokeReplyError::NoTarget: {
            qDebug() << "Service::invokeFinished(): Error: no target" << endl;
            m_settings->setValue("active", false);
            break;
        }

        // There was a problem with the invocation request;
        // did we set all of the values correctly?
        case InvokeReplyError::BadRequest: {
            qDebug() << "Service::invokeFinished(): Error: bad request" << endl;
            m_settings->setValue("active", false);
            break;
        }

        // Something went completely
        // wrong inside the invocation request,
        // so find an alternate route
        case InvokeReplyError::Internal: {
            qDebug() << "Service::invokeFinished(): Error: internal" << endl;
            m_settings->setValue("active", false);
            break;
        }

        // Message received if the invocation request is successful
        default:
            qDebug() << "Service::invokeFinished(): " << m_invokeReply->error() <<endl;
            m_settings->setValue("active", false);
            break;
    }
    m_invokeReply->deleteLater();
}

void Service::onError()
{
    if(sender())
        sender()->deleteLater();

    qDebug()<<"Service::onError";
    bb::Application::instance()->requestExit();
}

void Service::quit()
{
    if(sender())
        sender()->deleteLater();

    bb::Application::instance()->requestExit();
}
