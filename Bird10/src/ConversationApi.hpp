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

#ifndef CONVERSATIONAPI_HPP_
#define CONVERSATIONAPI_HPP_

#include <src/TimelineBase.hpp>
#include <bb/cascades/ArrayDataModel>
#include <src/TwitterApi.hpp>


class ConversationApi: public TwitterApi
{
    Q_OBJECT

public:

    ConversationApi(QObject *parent = 0);

    Q_INVOKABLE void loadConversation(const QString& id);

signals:
    void conversationLoaded(int index);

private:

    void insertTweet(const QVariantMap& tweet);
    void insertTweetsFromThread(const QVariantMap& thread);

private slots:

    void conversationReceived();

private:
    QVariantMap m_tweets;
    QVariantMap m_users;
    QString m_id;
    int m_tweetsCount;
    int m_mainTweetIndex;
};

#endif /* CONVERSATIONAPI_HPP_ */
