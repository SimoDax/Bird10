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

#include <src/DiscoverApi.hpp>
#include <src/QJson4/QJsonDocument.h>

DiscoverApi::DiscoverApi(QObject* parent) : TwitterApi(parent)
{
}

DiscoverApi::~DiscoverApi()
{
    // TODO Auto-generated destructor stub
}


void DiscoverApi::requestSearch(const QString& query, const QString& filter, const QString& cursorBottom)
{
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    m_query = query;
    m_filter = filter;

    QString url = ("https://api.twitter.com/2/search/adaptive.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("q", query.toUtf8()));
    par.append(O0RequestParameter("count", "40"));
    par.append(O0RequestParameter("include_user_entities", "true"));


    if(!filter.isEmpty() && filter != "recent")
        par.append(O0RequestParameter("result_filter", filter.toUtf8()));
    else if (filter == "recent")
        par.append(O0RequestParameter("tweet_search_mode", "live"));

    if(!cursorBottom.isEmpty())
        par.append(O0RequestParameter("cursor", cursorBottom.toUtf8()));
    else
        tweetModel_->clear();

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onSearchReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void DiscoverApi::requestOlderTweets(){
    requestSearch(m_query, m_filter, m_cursorBottom);
}

void DiscoverApi::onSearchReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();


//            QFile file("./data/tweets.json");
//            file.open(QIODevice::WriteOnly);
//            if(file.isOpen()){
//                file.write(reply->data());
//                file.close();
//            }


    m_users = content["globalObjects"].toMap()["users"].toMap();
    m_tweets = content["globalObjects"].toMap()["tweets"].toMap();

    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();


    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().find("addEntries") != instructions[i].toMap().end()){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
            qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
                QString id = entries[i].toMap()["entryId"].toString();

                if(id.contains("sq-I-t"))
                    insertTweet(entries[i].toMap()["content"].toMap());
                else if (id.contains("sq-cursor-bottom"))
                    m_cursorBottom = entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString();
            }
        }

        // Update cursor bottom while scrolling down
        if (instructions[i].toMap().keys().contains("replaceEntry"))
        {
            QVariantMap entry = instructions[i].toMap()["replaceEntry"].toMap()["entry"].toMap();
            if(entry["entryId"] == "sq-cursor-bottom")
                m_cursorBottom = entry["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString();
        }
        else if(instructions[i].toMap().keys().contains("terminateTimeline")){
            terminateTimeline(instructions[i].toMap());
        }

        qDebug()<<instructions[i].toMap().keys();
    }

    emit tweetModelChanged();

    m_users.clear();
    m_tweets.clear();

    reply->deleteLater();
}

void DiscoverApi::insertTweet(const QVariantMap& tweet)
{
    QVariantMap tweetContent = tweet["item"].toMap()["content"].toMap()["tweet"].toMap();

    if(tweetContent.keys().contains("promotedMetadata"))
        return;

    QString id = tweetContent["id"].toString();

    QVariantMap tweetObject = parseTweetV2(realTweet(m_tweets[id].toMap()), m_tweets, m_users);

    //qDebug()<<tweetObject["user_id_str"].toString();

    tweetModel_->append(tweetObject);
}

