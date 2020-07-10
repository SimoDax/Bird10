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

#ifndef DISCOVERAPI_HPP_
#define DISCOVERAPI_HPP_


#include <src/twitterapi.hpp>

class DiscoverApi: public TwitterApi
{
    Q_OBJECT
public:

    DiscoverApi(QObject* parent = 0);
    virtual ~DiscoverApi();

    Q_INVOKABLE void requestSearch(const QString& query, const QString& filter = QString(), const QString& cursorBottom = QString());
    Q_INVOKABLE void requestOlderTweets();

protected slots:

    void onSearchReceived();


protected:

    void insertTweet(const QVariantMap& tweet);

protected:

    QVariantMap m_users, m_tweets;
    QString m_query, m_filter, m_cursorBottom;
};

#endif /* DISCOVERAPI_HPP_ */
