/*
 * ListApi.cpp
 *
 *  Created on: 11/ago/2020
 *      Author: Simone
 */

#include <src/ListApi.hpp>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonArray.h>

ListApi::ListApi(QObject *parent): TwitterApi(parent)
{
    // TODO Auto-generated constructor stub
    m_listsModel = new bb::cascades::ArrayDataModel(this);
}

void ListApi::requestTweets(QString max_id, QString since_id)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/lists/statuses.json?tweet_mode=extended&count=40&list_id=" + m_listId);
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "40"));
    par.append(O0RequestParameter("list_id", m_listId.toUtf8()));

    if(!max_id.isEmpty()){
        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
    }
    if(!since_id.isEmpty()){
        url += "&since_id=" + QString("%1").arg(since_id.toLongLong()-1);
        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()-1).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(!max_id.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(olderTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(tweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ListApi::requestLists()
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/lists/list.json");

    CurlEasy* reply = requestor->get(url, QList<O0RequestParameter>());

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onLists()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void ListApi::onLists()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonArray jsonArray = jsonResponse.array();

    if(!jsonArray.isEmpty()){
        foreach (const QJsonValue &v, jsonArray) {
            QVariantMap item = v.toObject().toVariantMap();

            m_listsModel->append(item);
        }

        emit listsModelChanged();
    }
}
