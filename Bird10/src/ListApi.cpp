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

#include <src/ListApi.hpp>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonArray.h>

ListApi::ListApi(QObject *parent): TwitterApi(parent)
{
    m_listsModel = new bb::cascades::ArrayDataModel(this);
}

void ListApi::requestTweets(Direction dir)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/timeline/list.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("ranking_mode", "reverse_chronological"));
    par.append(O0RequestParameter("count", "40"));
    par.append(O0RequestParameter("list_id", m_listId.toUtf8()));

    if(dir == BOTTOM){
        par.append(O0RequestParameter("cursor", QString("%1").arg(tweetModel_->cursorBottom()).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(dir == BOTTOM)
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
