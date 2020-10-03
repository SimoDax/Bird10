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

#ifndef LISTAPI_HPP_
#define LISTAPI_HPP_

#include "twitterapi.hpp"

class ListApi: public TwitterApi
{
    Q_OBJECT
public:
    /// List of lists (no pun intended)
    Q_PROPERTY(bb::cascades::ArrayDataModel *listsModel READ listsModel NOTIFY listsModelChanged)
    inline bb::cascades::ArrayDataModel *listsModel(){ return m_listsModel; };

    Q_PROPERTY(QString listId READ listId WRITE setListId NOTIFY listIdChanged)
    inline QString listId(){ return m_listId; };
    inline void setListId(QString listId){ m_listId = listId; emit listIdChanged(); };

    ListApi(QObject* parent = 0);

    Q_INVOKABLE void requestTweets(QString max_id = QString(), QString since_id = QString());

    Q_INVOKABLE void requestLists();

signals:
    void listsModelChanged();
    void listIdChanged();

protected slots:
    void onLists();

protected:
    bb::cascades::ArrayDataModel* m_listsModel;
    QString m_listId;
};

#endif /* LISTAPI_HPP_ */
