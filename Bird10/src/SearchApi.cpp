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
#include <src/SearchApi.hpp>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonObject.h>
#include <QSqlDatabase>

SearchApi::SearchApi(QObject* parent) : TwitterApiBase(parent)
{
    m_trends = new bb::cascades::ArrayDataModel(this);
    m_users = new bb::cascades::ArrayDataModel(this);
}

SearchApi::~SearchApi()
{
    // TODO Auto-generated destructor stub
}

void SearchApi::requestTrends()
{
    if (!authenticator_ || !authenticator_->linked()) {
        m_trends->clear();
        emit trendListChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/guide.json");
    QList<O0RequestParameter> par = basicGetParameters();
//    par.append(O0RequestParameter("count", "20"));
//    par.append(O0RequestParameter("include_user_entities", "false"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onTrendsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void SearchApi::onTrendsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();


    m_trends->clear();

    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().find("addEntries") != instructions[i].toMap().end()){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
            qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
                QString id = entries[i].toMap()["entryId"].toString();

                if(id.contains("trends"))
                    insertTrends(entries[i].toMap()["content"].toMap());

                //TODO: implement cursor
            }
        }
        //TODO: implement terminate timeline
    }

    emit trendListChanged();

    reply->deleteLater();
}


void SearchApi::insertTrends(const QVariantMap& trends){
    QVariantList trendList = trends["timelineModule"].toMap()["items"].toList();

    for(int i = 0; i<trendList.size(); i++){

        if(trendList[i].toMap()["item"].toMap()["content"].toMap()["trend"].toMap().keys().contains("promotedMetadata"))
            continue;

        m_trends->append(trendList[i].toMap()["item"].toMap()["content"].toMap()["trend"].toMap()["name"].toString());
    }
}

void SearchApi::searchUser(const QString& text){
    if (!authenticator_ || !authenticator_->linked()) {
        m_users->clear();
        emit userListChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/search/typeahead.json?q="+QUrl::toPercentEncoding(text)+"&src=search_box&result_type=users");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("q", text.toUtf8()));
    par.append(O0RequestParameter("src", "search_box"));
    par.append(O0RequestParameter("result_type", "users"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onUserSearchReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void SearchApi::onUserSearchReceived(){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    m_users->clear();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap data = jsonResponse.object().toVariantMap();

    QVariantList users = data["users"].toList();
    for(int i = 0; i<users.size(); i++){
        m_users->append(users[i]);
    }

    emit userListChanged();

    reply->deleteLater();
}
