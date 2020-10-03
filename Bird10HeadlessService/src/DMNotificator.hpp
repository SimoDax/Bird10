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

#ifndef DMNOTIFICATOR_HPP_
#define DMNOTIFICATOR_HPP_

#include "Notificator.hpp"

class DMNotificator: public Notificator
{
    Q_OBJECT
public:
    DMNotificator(OXTwitter* authenticator, QSettings* settings, QObject* parent);

    void checkTwitterNotifications();

protected slots:
    void onNotifications();

protected:
    void sendNotification(const QString& body, const QString& conversation_id);
    void deleteNotification(const QString& conversation_id);
    void deleteAll();

    QString buildBody(const QVariantMap& conversation);
};

#endif /* DMNOTIFICATOR_HPP_ */
