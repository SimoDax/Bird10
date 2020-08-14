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

#ifndef DMAPI_HPP_
#define DMAPI_HPP_

#include "TwitterApiBase.hpp"
#include <bb/cascades/GroupDataModel>

using namespace bb::cascades;

//typedef QMap<QString, GroupDataModel*> GroupDataModelMap;  // moc doesn't like template syntax in q_property

class DMApi: public TwitterApiBase
{
    Q_OBJECT

public:

    /// Only contains unread conversations. Guaranteed.
    Q_PROPERTY(QVariantList conversations READ conversations NOTIFY inboxModelChanged)
    inline QVariantList conversations(){ return (m_conversations); };

    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    inline int unreadCount(){ return m_unreadCount; };

    Q_PROPERTY(qulonglong lastSeenId READ lastSeenId NOTIFY unreadCountChanged)
    inline qulonglong lastSeenId(){ return m_lastSeenId; };

    Q_PROPERTY(QString cursor READ cursor WRITE setCursor NOTIFY unreadCountChanged)
    inline QString cursor(){ return m_cursor; };
    void setCursor(QString cursor){ m_cursor = cursor; };

    DMApi(QObject *parent=0);
    virtual ~DMApi();

    Q_INVOKABLE void loadInboxInitialState();
    Q_INVOKABLE void pollUpdates(const QString& conversationId = QString());

signals:
    void inboxModelChanged();
    void unreadCountChanged();

protected slots:
    void onInboxInitialStateReceived();
    void onUpdates();
    void onUpdatesFailed(CURLcode c);

protected:
    void insertConversations(const QVariantMap& content);


protected:
    QVariantList m_conversations;

    QString m_cursor;
    qulonglong m_lastSeenId;
    int m_unreadCount;
    bool m_polling;
};

#endif /* DMAPI_HPP_ */
