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

#include <src/DMApi.hpp>
#include <src/QJson4/QJsonObject.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/MediaUploader.hpp>
#include <iostream>

bool lessThan(const QVariant& l, const QVariant& r);

DMApi::DMApi(QObject *parent) : TwitterApiBase(parent)
{

    m_inbox = new GroupDataModel(QStringList() << "time", this);
    m_inbox -> setSortedAscending(false);
    m_inbox -> setGrouping(ItemGrouping::None);
    m_polling = 0;

}

DMApi::~DMApi()
{
}

void DMApi::loadInboxInitialState()
{
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    QString url = ("https://api.twitter.com/1.1/dm/inbox_initial_state.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("include_groups", "true"));
    par.append(O0RequestParameter("include_inbox_timelines", "true"));
    par.append(O0RequestParameter("supports_reactions", "false"));
    par.append(O0RequestParameter("filter_low_quality", "true"));
    par.append(O0RequestParameter("include_quality", "all"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onInboxInitialStateReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DMApi::onInboxInitialStateReceived(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

//    saveReply(reply, "dm");

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap()["inbox_initial_state"].toMap();
    m_lastSeenId = content["last_seen_event_id"].toULongLong();
    qDebug()<<"last seen id: "<<m_lastSeenId;

    m_cursor = content["cursor"].toString();

    // Create conversation containers
    foreach(const QString& conversation_id, content["conversations"].toMap().keys()){

        m_conversations.insert(conversation_id, new Conversation());
    }

    // Populate containers with messages
    foreach(const QVariant& message, content["entries"].toList())
        insertMessage(message.toMap(), content["users"].toMap());


    // Load inbox with latest message for each conversation
    foreach(Conversation* conversation, m_conversations.values()){
        updateInboxPreview(conversation, content);

        m_inbox->insert(conversation);
    }

    emit inboxModelChanged();

//    QVariantList conversations = content["conversations"].toMap().values();
//    qSort(conversations.begin(), conversations.end(), lessThan);
}

//bool lessThan(const QVariant& l, const QVariant& r){
//    return l.toMap()["sort_timestamp"].toULongLong() > r.toMap()["sort_timestamp"].toULongLong();
//}


void DMApi::sendDM(const QString& text, const QString& conversationId, const QString& mediaId)
{
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    QString url = ("https://api.twitter.com/1.1/dm/new.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("text", text.toUtf8()));
    par.append(O0RequestParameter("conversation_id", conversationId.toUtf8()));
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("tweet_mode", "extended"));

    if(!mediaId.isEmpty())
        par.append(O0RequestParameter("media_id", mediaId.toUtf8()));


    CurlEasy* reply = requestor->post(url, par);


    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onSentDM()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void DMApi::onSentDM(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    // Messages are now inserted when Twitter returns them in the updates -- it would return them anyway
//    insertMessage(content["entries"].toList()[0].toMap(), content["users"].toMap());

    pollUpdates(content["entries"].toList().value(0).toMap()["message"].toMap()["conversation_id"].toString());

    emit scrollToLast(true);

    reply->deleteLater();
}

void DMApi::pollUpdates(const QString& conversationId)
{
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    if(m_polling)
        return;

    m_polling = true;

    QString url = ("https://api.twitter.com/1.1/dm/user_updates.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("cursor", m_cursor.toUtf8()));
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("include_groups", "true"));
    par.append(O0RequestParameter("supports_reactions", "false"));

    if(!conversationId.isEmpty())
        par.append(O0RequestParameter("active_conversation_id", conversationId.toUtf8()));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onUpdates()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onUpdatesFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void DMApi::onUpdates(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap()["user_events"].toMap();

    m_cursor = content["cursor"].toString();

    qDebug()<<"DMApi::onUpdates: cursor: "<<m_cursor<<" id: "<<content["last_seen_event_id"];

    if(content["last_seen_event_id"].toULongLong() != m_lastSeenId){    // Some messages have been seen through another client!
        m_lastSeenId = content["last_seen_event_id"].toULongLong();

        // Now we need to update all the previews...
        foreach(Conversation* c, m_conversations){
            if(!c)  continue;

            QVariantMap preview = c->previewData();
//            qDebug()<<c->previewData()["status"]<<" "<<c->previewData()["sort_event_id"].toULongLong();
            if(preview["status"] == 1 && preview["sort_event_id"].toULongLong() <= m_lastSeenId){
                preview["status"] = 0;
                c->setPreviewData(preview);
            }
//            emit inboxModelChanged();
        }

        emit inboxModelChanged();
    }

    if(content["entries"].toList().size()){

        //m["id"].toULongLong() > conversation->data(conversation->first()).toMap()["id"].toULongLong()
        foreach(const QVariant& message, content["entries"].toList()){
            insertMessage(message.toMap(), content["users"].toMap());
            updateInboxPreview(getConversationFromMessage(message.toMap()), content);
        }

        emit inboxModelChanged();
        emit currentConversationChanged();  // not necessarily true if a conversation isn't open..
        emit scrollToLast(false);
    }

    m_polling = false;
    reply->deleteLater();
}

void DMApi::onUpdatesFailed(CURLcode c){
    onRequestFailed(c);

    m_polling = false;
}


void DMApi::requestOlderDM(const QString& conversationId){
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    Conversation * conversation = m_conversations.value(conversationId);
    if(!conversation)
        return;

    GroupDataModel* c = conversation->messageModel();

    QString max_id = QString::number(c->data(c->before(c->last())).toMap()["id"].toULongLong()-1);    // before last because last is spinner element
                                                                                                      //TODO: what happens if the spinner is removed because the conversation has no more old messages?

    QString url = ("https://api.twitter.com/1.1/dm/conversation/"+ conversationId +".json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("max_id", max_id.toUtf8()));
    par.append(O0RequestParameter("dm_users", "false"));
    par.append(O0RequestParameter("include_groups", "true"));
    par.append(O0RequestParameter("include_inbox_timelines", "true"));
    par.append(O0RequestParameter("supports_reactions", "false"));
    par.append(O0RequestParameter("filter_low_quality", "true"));
    par.append(O0RequestParameter("include_quality", "all"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onOlderDM()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DMApi::onOlderDM(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap()["conversation_timeline"].toMap();

    if(content["status"].toString() == "AT_END"){
        //FIXME we're supposing this reply arrives so quickly that the user has not changed conversation, since there doesn't seem to be a way to get the conversation id in this case..
        if(m_currentConversation->data(m_currentConversation->last()).toMap()["time"] == 0) // Only if the spinner hasn't been removed by previous requestOlderDM
            m_currentConversation->removeAt(m_currentConversation->last());
        return;
    }

    bool firstTime = true;
    Conversation* conversation;
    foreach(const QVariant& message, content["entries"].toList()){
        // No need to extract for each message the conversation, it's the same for all
        if(firstTime){
            QVariantMap m = message.toMap()["message"].toMap();
            conversation = m_conversations.value(m["conversation_id"].toString());
            firstTime = false;
        }

        if(conversation){
            insertMessageInConversation(message.toMap(), content["users"].toMap(), conversation);
        }
    }

    emit currentConversationChanged();

    reply->deleteLater();
}

void DMApi::markRead(const QString& conversationId){
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    qDebug()<<"DMApi::markRead";

    Conversation* conversation = m_conversations.value(conversationId);
    if(!conversation)
        return;

    QVariantMap preview = conversation->previewData();
    if(preview["status"] == 1){
        preview["status"] = 0;
        conversation->setPreviewData(preview);
    }
    emit inboxModelChanged();

    GroupDataModel* c = conversation->messageModel();

    QString last_id = c->data(c->first()).toMap()["id"].toString();

    QString url = ("https://api.twitter.com/1.1/dm/conversation/"+ conversationId +"/mark_read.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("last_read_event_id", last_id.toUtf8()));
    par.append(O0RequestParameter("send_error_codes", "true"));

    CurlEasy* reply = requestor->post(url, par);

    connect(reply, SIGNAL(done(CURLcode)), reply, SLOT(deleteLater()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DMApi::insertMessage(const QVariantMap& message, const QVariantMap& users){
    QVariantMap m = message["message"].toMap();

    if(m["conversation_id"].toString().isEmpty()){
        return;
    }

    Conversation* conversation = m_conversations.value(m["conversation_id"].toString());

    if(!conversation)
        conversation = createConversation(m["conversation_id"].toString());

    insertMessageInConversation(message, users, conversation);
}

void DMApi::insertMessageInConversation(const QVariantMap& message_, const QVariantMap& users, Conversation* conversation){
    QVariantMap message;
    QVariantMap m = message_["message"].toMap();
    QString sender_id = m["message_data"].toMap()["sender_id"].toString();
    QVariantMap user = users[sender_id].toMap();

    message["avatar_image"] = user["profile_image_url"];
    message["name"] = user["name"];
    message["screen_name"] = user["screen_name"];
//    message["text"] = m["message_data"].toMap()["text"].toString();
    message["time"] = m["time"].toULongLong();   // for ordering
    QDateTime messageTime = QDateTime::fromMSecsSinceEpoch(m["time"].toULongLong());
    message["date"] = messageTime.date() == QDate::currentDate() ? messageTime.toString("h:mm") : messageTime.toString("d MMM yyyy, h:mm");
    message["conversation_id"] = m["conversation_id"];
    message["id"] = m["id"];
    message["sender_id"] = m["message_data"].toMap()["sender_id"];
    message["recipient_id"] = m["message_data"].toMap()["recipient_id"];

    // Rewrite tweet, use utf-32 to count emoji only once
    std::wstring text = m["message_data"].toMap()["text"].toString().toStdWString();
    int offset = 0;

    if(m["message_data"].toMap().keys().contains("attachment"))
        if(m["message_data"].toMap()["attachment"].toMap().keys().contains("photo")){
            QVariantMap photo = m["message_data"].toMap()["attachment"].toMap()["photo"].toMap();
            message["photoUrlSmall"] = photo["media_url"].toString().replace("https:", "http:");
            int beginIndex = m["message_data"].toMap()["attachment"].toMap()["photo"].toMap()["indices"].toList()[0].toInt();
            int endIndex = m["message_data"].toMap()["attachment"].toMap()["photo"].toMap()["indices"].toList()[1].toInt();
            text.erase(beginIndex, endIndex-beginIndex);
            offset = - (endIndex - beginIndex);
        }

    QVariantList urls = m["message_data"].toMap()["entities"].toMap()["urls"].toList();

    for(int i = 0; i<urls.size(); i++){
        int beginIndex = urls[i].toMap()["indices"].toList()[0].toInt();
//        std::wcerr<<text<<" "<<text.size();
        if(beginIndex+offset >= text.size())   //link already cropped out by display_text_range
            continue;
        int endIndex = urls[i].toMap()["indices"].toList()[1].toInt();

        QString htmlUrl = urls[i].toMap()["expanded_url"].toString();   // coupled with content.flags in textbox

        text.replace(beginIndex+offset, endIndex-beginIndex, htmlUrl.toStdWString());

        offset += htmlUrl.length() - (endIndex - beginIndex);   //keeps track of indexes changes due to html insertion

    }
    message["text"] = QString::fromStdWString(text);

    conversation->insertMessage(message);

//    qDebug()<<message["text"];
}

void DMApi::requestBadgeCount()
{
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }

    qDebug()<<"DMApi::requestBadgeCount";

    QString url = ("https://api.twitter.com/2/badge_count/badge_count.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("supports_ntab_urt", "true"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onBadgeCount()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DMApi::onBadgeCount()
{
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    m_unreadCount = jsonResponse.object().toVariantMap()["dm_unread_count"].toInt();

    emit unreadCountChanged();

    reply->deleteLater();
}

/*
 * Called by:
 *
 *  - Qml when the user switches to the Messages tab
 *  - Qml when the user is looking at the Messages tab and new messages arrive, through the inbox changed slot
 *
 */
void DMApi::updateLastSeenId()
{
    if (!authenticator_ || !authenticator_->linked()) {
            m_inbox->clear();
            emit inboxModelChanged();
            return;
    }
    qDebug()<<"DMApi::updateLastSeenId";

    QString url = ("https://api.twitter.com/1.1/dm/update_last_seen_event_id.json");
    QList<O0RequestParameter> par = basicGetParameters();

    QObject* o = qvariant_cast<QObject*>(m_inbox->data(m_inbox->first()));
    Conversation* c = qobject_cast<Conversation*>(o);

    if(!c)      // no conversations for this user :(
        return;

    qulonglong id = c->previewData()["sort_event_id"].toULongLong();    //id of latest message in inbox, sort_event_id contains latest message id for each conversation
    par.append(O0RequestParameter("last_seen_event_id", QByteArray::number(id)));
    par.append(O0RequestParameter("trusted_last_seen_event_id", QByteArray::number(id)));

    CurlEasy* reply = requestor->post(url, par);

    connect(reply, SIGNAL(done(CURLcode)), reply, SLOT(deleteLater()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

    m_lastSeenId = id;

    m_unreadCount = 0;
    emit unreadCountChanged();
}

void DMApi::uploadMedia(const QString& filePath)
{
    MediaUploader* mu = new MediaUploader(authenticator_, this);
    connect(mu, SIGNAL(uploadComplete(QString)), this, SLOT(onMediaUploaded(QString)));
    connect(mu, SIGNAL(error(QString)), this, SLOT(onMediaUploadFailed(QString)));
    mu->setMediaContext(MediaUploader::DM);
    mu->uploadPictures(QStringList(filePath));
}

void DMApi::onMediaUploaded(const QString& media_id){

    emit mediaUploaded(media_id);
    sender()->deleteLater();
}

void DMApi::onMediaUploadFailed(const QString& error){

    emit mediaUploadError(error);
    sender()->deleteLater();
}

void DMApi::updateInboxPreview(Conversation* conversation, const QVariantMap& content){
    if(!conversation)
        return;

    QVariantMap m = conversation->messageModel()->data(conversation->messageModel()->first()).toMap();
    QVariantMap preview;
    QString conv_id = m["conversation_id"].toString();
    preview["id"] = conv_id;
    preview["sort_event_id"] = m["id"].toULongLong();    // latest message id
    if(conv_id.contains("-")){
        // it's a one-to-one chat
        preview["type"] = "oneToOne";

        // find who is on the other end, let's try with the sender
        //QString other_id = m["message_data"].toMap()["sender_id"].toString();
        QString other_id = m["sender_id"].toString();
        preview["status"] = m["id"].toULongLong() > m_lastSeenId ? 1 : 0;
        if(other_id == authenticator_->extraTokens()["user_id"]){
            // nope, the other person is the recipient
//                other_id = m["message_data"].toMap()["recipient_id"].toString();
            other_id = m["recipient_id"].toString();
            preview["status"] = 2;
        }

        QVariantMap user = content["users"].toMap()[other_id].toMap();
        preview["avatar_image"] = user["profile_image_url"];
        preview["name"] = user["name"];
        preview["screen_name"] = user["screen_name"];
        preview["message"] = m["text"].toString();
        preview["time"] = m["time"].toULongLong();   // for ordering
        preview["date"] = QDateTime::fromMSecsSinceEpoch(m["time"].toULongLong()).toString("d/M/yy");
    }
    else{
        // it's a group chat
        preview["type"] = "group";

        QString sender = m["sender_id"].toString();
        if(sender == authenticator_->extraTokens()["user_id"])
            preview["status"] = 2;
        else
            preview["status"] = m["id"].toULongLong() > m_lastSeenId ? 1 : 0;


        QVariantMap conv = content["conversations"].toMap()[conv_id].toMap();
        preview["avatar_image"] = conv["avatar_image_https"];
        preview["name"] = conv["name"];
        preview["screen_name"] = "";
        preview["message"] = m["message_data"].toMap()["text"].toString();
        preview["time"] = m["time"].toULongLong();   // for ordering
        preview["date"] = QDateTime::fromMSecsSinceEpoch(m["time"].toULongLong()).toString("d/M/yy");
    }
    conversation->setPreviewData(preview);

}

Conversation* DMApi::getConversationFromMessage(const QVariantMap& message)
{
    QVariantMap m = message["message"].toMap();
    return m_conversations.value(m["conversation_id"].toString());
}

QString DMApi::getConversationIdFromUser(const QString& user)
{
    for(int i = 0; i<m_conversations.keys().size(); i++){
        if(m_conversations.keys()[i].contains(user))
            return m_conversations.keys()[i];
    }

    // it's a new conversation!
    QString conversation_id = authenticator_->extraTokens()["user_id"].toString() + "-" + user;
    createConversation(conversation_id);
    return conversation_id;
}

Conversation* DMApi::createConversation(const QString& conversation_id)
{
    Conversation* conversation = new Conversation();
    m_conversations.insert(conversation_id, conversation);
    m_inbox->insert(conversation);

    return conversation;
}

void DMApi::clear(){
    m_inbox->clear();
    m_conversations.clear();
    emit inboxModelChanged();
    m_cursor="";
    m_lastSeenId = 0;
}
