/*
 * ListApi.hpp
 *
 *  Created on: 11/ago/2020
 *      Author: Simone
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
