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

#ifndef NOTIFICATOR_HPP_
#define NOTIFICATOR_HPP_

#include <QObject>
#include <QSettings>
#include "o2/oxtwitter.h"

#ifdef QT_DEBUG
#define NOTIFICATION_TITLE "Bird10 (debug)"
#else
#define NOTIFICATION_TITLE "Bird10"
#endif


class Notificator : public QObject
{
    Q_OBJECT
public:
    Notificator(OXTwitter* authenticator, QSettings* settings, QObject* parent);

    void checkTwitterNotifications();

signals:
    void done();

protected slots:
    void onNotifications();
    void onError();

protected:
    void sendNotification(const QString& body, const QString& sortIndex);
    void deleteNotification(const QString& key);

    QString buildBody(const QVariantMap& notification);

protected:

    OXTwitter* m_authenticator;
    QSettings* m_settings;
};

#endif /* NOTIFICATOR_HPP_ */
