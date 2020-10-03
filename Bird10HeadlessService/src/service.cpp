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

#include "logger/logger.h"


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
    LOG("Service::handleInvoke: ACTION IS " + request.action());


    // Check if we can hit twitter
    if(!m_authenticator->linked()){
        // deregister and close
        clearNotifications();
        clearTimer();
        clearHeartbeat();
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
        registerHeartbeat();
        registerTimer();
        checkNotifications();

    }
    else if(request.action().compare("bb.action.system.TIMER_FIRED") == 0){

        // Sometimes the STOP action happens just before the timer fires and the invocation platform can't deregister it quickly enough
        // so we should check if the sevice is supposed to be in active state before scheduling the next execution
        if(m_settings->value("active", false).toBool() == false){
            qDebug()<<"Service::handleInvoke: QUITTING BECAUSE active = false !";
            LOG("Service::handleInvoke: QUITTING BECAUSE active = false !");
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
        clearHeartbeat();
        bb::Application::instance()->requestExit();
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
    LOG("Service::registerTimer: SETTING TIMER");

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
    LOG("Service::stop");

    m_invokeManager->deregisterTimer("com.simodax.bird10.TIMER");
    m_settings->setValue("active", false);
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
    LOG("Service::checkDM");

    DMNotificator* n = new DMNotificator(m_authenticator, m_settings, this);

    bool ok = connect(n, SIGNAL(done()), this, SLOT(quit()));

    n->checkTwitterNotifications();
}

void Service::onInvokeResult(){   //helper function pasted stright from docs, saves a lot of headaches
    // Check for errors
    switch(m_invokeReply->error()) {
        case InvokeReplyError::None: {
            qDebug() << "Service::invokeFinished(): No error" << endl;
            LOG("Service::invokeFinished(): No error");
            m_settings->setValue("active", true);
            break;
        }
        // Invocation could not find the target;
        // did we use the right target ID?
        case InvokeReplyError::NoTarget: {
            qDebug() << "Service::invokeFinished(): Error: no target" << endl;
            LOG("Service::invokeFinished(): Error: no target");
            m_settings->setValue("active", false);
            break;
        }

        // There was a problem with the invocation request;
        // did we set all of the values correctly?
        case InvokeReplyError::BadRequest: {
            // The heartbeat timer is already firing at that time, request rejected
            qDebug() << "Service::invokeFinished(): Error: bad request" << endl;
            LOG("Service::invokeFinished(): Error: bad request");
            m_settings->setValue("active", true);
            break;
        }

        // Something went completely
        // wrong inside the invocation request,
        // so find an alternate route
        case InvokeReplyError::Internal: {
            qDebug() << "Service::invokeFinished(): Error: internal" << endl;
            LOG("Service::invokeFinished(): Error: internal");
            m_settings->setValue("active", false);
            break;
        }

        // Message received if the invocation request is successful
        default:
            qDebug() << "Service::invokeFinished(): Error: " << m_invokeReply->error() <<endl;
            LOG("Service::invokeFinished(): Error: "+ m_invokeReply->error());
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

void Service::registerHeartbeat()
{
    qDebug()<<"Service::registerHeartbeat: SETTING HEARTBEAT";
    LOG("Service::registerTimer: SETTING HEARTBEAT");

    InvokeRecurrenceRule recurrenceRule(bb::system::InvokeRecurrenceRuleFrequency::Minutely);
    recurrenceRule.setInterval(6);
    InvokeDateTime startTime(QDate::currentDate(), QTime::currentTime().addSecs(60*6), "");
    recurrenceRule.setStartTime(startTime);

    // Create the timer request
    InvokeTimerRequest timer_request("com.simodax.bird10.HEARTBEAT", recurrenceRule, "com.simodax.Bird10HeadlessService");

    m_invokeReply = m_invokeManager->registerTimer(timer_request);
    bool ok = connect(m_invokeReply, SIGNAL(finished()), this, SLOT(onInvokeResult()));

    qDebug()<<"Service::start: TIMER SET";
}

void Service::clearHeartbeat()
{
    qDebug()<<"Service::clearHeartbeat";
    LOG("Service::clearHeartbeat");

    m_invokeManager->deregisterTimer("com.simodax.bird10.HEARTBEAT");
    m_settings->setValue("active", false);
}
