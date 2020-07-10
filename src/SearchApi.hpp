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

#ifndef SEARCHAPI_HPP_
#define SEARCHAPI_HPP_

#include "TwitterApiBase.hpp"
#include <bb/cascades/ArrayDataModel>

class SearchApi: public TwitterApiBase
{
    Q_OBJECT
public:

    Q_PROPERTY( bb::cascades::ArrayDataModel* trends READ getTrends NOTIFY trendListChanged  )
    bb::cascades::ArrayDataModel* getTrends(){ return m_trends; } ;

    Q_PROPERTY( bb::cascades::ArrayDataModel* users READ getUsers RESET clearUsers NOTIFY userListChanged  )
    bb::cascades::ArrayDataModel* getUsers(){ return m_users; } ;
    Q_INVOKABLE void clearUsers(){ m_users->clear(); emit userListChanged(); } ;

    SearchApi(QObject* parent = 0);
    virtual ~SearchApi();

    Q_INVOKABLE void requestTrends();
    Q_INVOKABLE void searchUser(const QString& text);

protected slots:

    void onTrendsReceived();
    void onUserSearchReceived();

signals:

    void trendListChanged();
    void userListChanged();

protected:

    void insertTrends(const QVariantMap& trends);

protected:

    bb::cascades::ArrayDataModel* m_trends, * m_users;
};

#endif /* SEARCHAPI_HPP_ */
