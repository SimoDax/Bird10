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
    void registerHeartbeat();
    void clearHeartbeat();

    void clearNotifications();


    bb::system::InvokeManager * m_invokeManager;
    bb::system::InvokeReply* m_invokeReply;

    OXTwitter* m_authenticator;

    QSettings* m_settings;

    bool m_running;
};

#endif /* SERVICE_H_ */
