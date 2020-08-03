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

#ifndef CONVERSATION_HPP_
#define CONVERSATION_HPP_

#include <QObject>
#include <bb/cascades/GroupDataModel>

class Conversation: public QObject
{
    Q_OBJECT
public:

    Q_PROPERTY(QVariantMap previewData READ previewData WRITE setPreviewData NOTIFY previewDataChanged FINAL)
    inline QVariantMap previewData(){ return m_previewData; };
    inline void setPreviewData(const QVariantMap& previewData){ m_previewData = previewData; emit previewDataChanged(); };

    Q_PROPERTY(bb::cascades::GroupDataModel* messageModel READ messageModel NOTIFY messageModelChanged FINAL)
    inline bb::cascades::GroupDataModel* messageModel(){ return m_messageModel; };

    // shortcut to time field of previewData, needed by the inbox GroupDataModel pointing to the conversation for ordering
    Q_PROPERTY(qulonglong time READ time NOTIFY previewDataChanged FINAL)
    inline qulonglong time(){ return m_previewData["time"].toULongLong(); };


    Conversation(QObject *parent = 0);
    ~Conversation();

    void insertMessage(const QVariantMap& message);


signals:
    void previewDataChanged();
    void messageModelChanged();

private:
    bb::cascades::GroupDataModel* m_messageModel;
    QVariantMap m_previewData;
};

Q_DECLARE_METATYPE(Conversation*)

#endif /* CONVERSATION_HPP_ */
