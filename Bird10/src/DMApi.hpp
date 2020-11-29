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
#include <src/Conversation.hpp>

using namespace bb::cascades;

//typedef QMap<QString, GroupDataModel*> GroupDataModelMap;  // moc doesn't like template syntax in q_property

class DMApi: public TwitterApiBase
{
    Q_OBJECT

public:
    Q_PROPERTY(bb::cascades::GroupDataModel* inbox READ inbox NOTIFY inboxModelChanged)
    inline bb::cascades::GroupDataModel* inbox(){ return m_inbox; };

    #define COMMA ,     // c++ doesn't like template syntax in macros
    Q_PROPERTY(QMap<QString COMMA Conversation*> conversations READ conversations NOTIFY inboxModelChanged)
    inline QMap<QString, Conversation*> conversations(){ return (m_conversations); };

    Q_PROPERTY(bb::cascades::GroupDataModel* currentConversation READ currentConversation RESET resetCurrentConversation NOTIFY currentConversationChanged)
    inline bb::cascades::GroupDataModel* currentConversation(){ return m_currentConversation; };
    Q_INVOKABLE inline void resetCurrentConversation(){ m_currentConversation = nullptr; emit currentConversationChanged(); };

    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    inline int unreadCount(){ return m_unreadCount; };


    DMApi(QObject *parent=0);
    virtual ~DMApi();

    Q_INVOKABLE void loadInboxInitialState();
    Q_INVOKABLE void sendDM(const QString& text, const QString& conversationId, const QString& mediaId = QString());
    Q_INVOKABLE void uploadMedia(const QString& filePath);
    Q_INVOKABLE void pollUpdates(const QString& conversationId = QString());
    Q_INVOKABLE void requestOlderDM(const QString& conversationId);
    Q_INVOKABLE void requestBadgeCount();
    Q_INVOKABLE void markRead(const QString& conversationId);
    Q_INVOKABLE void updateLastSeenId();
    Q_INVOKABLE inline GroupDataModel* getConversationFromId(const QString& id){ Conversation* c = m_conversations.value(id); return c ? c->messageModel() : nullptr; };
    Q_INVOKABLE inline void setCurrentConversation(const QString& id){ m_currentConversation = m_conversations.value(id)->messageModel(); emit currentConversationChanged();}
    Q_INVOKABLE QString getConversationIdFromUser(const QString& user);
    Q_INVOKABLE void clear();

signals:
    void inboxModelChanged();
    void currentConversationChanged();
    void scrollToLast(bool forceScroll);
    void unreadCountChanged();
    void mediaUploaded(const QString& media_id);
    void mediaUploadError(const QString& error);

protected slots:
    void onInboxInitialStateReceived();
    void onSentDM();
    void onUpdates();
    void onUpdatesFailed(CURLcode c);
    void onOlderDM();
    void onBadgeCount();
    void onMediaUploaded(const QString& media_id);
    void onMediaUploadFailed(const QString& error);

protected:
    void insertMessage(const QVariantMap& message, const QVariantMap& users);
    void insertMessageInConversation(const QVariantMap& message_, const QVariantMap& users, Conversation* conversation);
    Conversation* getConversationFromMessage(const QVariantMap& message);
    Conversation* createConversation(const QString& conversation_id);

    void updateInboxPreview(Conversation* conversation, const QVariantMap& users);


protected:
    QMap<QString, Conversation*> m_conversations;
    GroupDataModel* m_inbox;
    GroupDataModel* m_currentConversation;

    QString m_cursor;
    qulonglong m_lastSeenId;
    int m_unreadCount;
    bool m_polling;
};

#endif /* DMAPI_HPP_ */
