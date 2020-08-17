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

#ifndef SERVICE_H_
#define SERVICE_H_

#include <QObject>
#include <QSettings>
#include <QTimer>
#include "o2/oxtwitter.h"

namespace bb {
    class Application;
    namespace platform {
        class Notification;
    }
    namespace system {
        class InvokeManager;
        class InvokeRequest;
        class InvokeReply;
    }
}

class Service: public QObject
{
    Q_OBJECT
public:
    Service();
    virtual ~Service() {}

    static QThread curlThread;

private slots:
    void handleInvoke(const bb::system::InvokeRequest &);
    void onTimeout();
    void onError();

    void onInvokeResult();
    void quit();

    void checkDM();

private:
    void checkNotifications();

    void registerTimer();
    void clearTimer();

    void clearNotifications();


    bb::system::InvokeManager * m_invokeManager;
    bb::system::InvokeReply* m_invokeReply;

    OXTwitter* m_authenticator;

    QSettings* m_settings;

    bool m_running;
};

#endif /* SERVICE_H_ */
