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

#ifndef NOTIFICATIONSAPI_HPP_
#define NOTIFICATIONSAPI_HPP_

#include "TwitterApiBase.hpp"
#include <bb/cascades/ArrayDataModel>

class NotificationsApi: public TwitterApiBase
{
    Q_OBJECT
public:
    Q_PROPERTY(bb::cascades::ArrayDataModel* notifications READ notifications NOTIFY notificationModelChanged)
    inline bb::cascades::ArrayDataModel* notifications(){ return m_notificationModel; };

    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    inline int unreadCount(){ return m_unreadCount; };
//    inline void setUnreadCount(int count){ m_unreadCount = count; emit unreadCountChanged(); };

    Q_PROPERTY(QString cursor READ cursor WRITE setCursor)
    inline QString cursor(){ return m_cursor; };
    inline void setCursor(const QString& c){ m_cursor = c; };

    Q_PROPERTY(bool shouldClearCache READ shouldClearCache)
    inline int shouldClearCache(){ return m_shouldClearCache; };

    Q_PROPERTY(QString unreadIndex READ unreadIndex)
    inline QString unreadIndex(){ return m_unreadIndex; };

    NotificationsApi(QObject *parent = 0);

    Q_INVOKABLE void requestNotifications();
    Q_INVOKABLE void updateUnreadIndex();
    Q_INVOKABLE void clearNotifications();


signals:
    void notificationModelChanged();
    void unreadCountChanged();

private slots:
    void onNotificationsReceived();

private:
    int insertNotification(const QString& n, int pos, const QString& sortIndex);
    int insertTweet(const QString& id, int pos, const QString& sortIndex);

private:
    QString m_cursor, m_unreadIndex;
    bb::cascades::ArrayDataModel* m_notificationModel;
    QVariantMap m_notifications, m_tweets, m_users;
    int m_unreadCount;
    bool m_shouldClearCache;
};

#endif /* NOTIFICATIONSAPI_HPP_ */
