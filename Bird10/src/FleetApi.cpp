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

#include <src/FleetApi.hpp>
#include "QJson4/QJsonDocument.h"
#include "QJson4/QJsonObject.h"

using namespace bb::cascades;

FleetApi::FleetApi(QObject* parent) : TwitterApiBase(parent)
    , m_fleetThreads(new ArrayDataModel(this))
    , m_allRead(true)
{
}

void FleetApi::requestFleets()
{
    if (!authenticator_ || !authenticator_->linked()) {
        m_fleetThreads->clear();
        return;
    }

    QString url = ("https://api.twitter.com/fleets/v1/fleetline");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("exclude_user_data", "true"));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onFleetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void FleetApi::markRead(const QString& fleet_thread_id, const QStringList& ids){
    foreach(const QString& id, ids){
        markRead(id);
    }

    // To reflect the read status of the fleets the user has just seen
    for(int i = 0; i<m_fleetThreads->size(); i++){
        if(m_fleetThreads->value(i).toMap()["fleet_thread_id"].toString() == fleet_thread_id)
            if(m_fleetThreads->value(i).toMap()["fleets"].toList().size() == ids.size()){
                QVariantMap thread = m_fleetThreads->value(i).toMap();
                thread["fully_read"] = true;
                m_fleetThreads->replace(i, thread);
            }

    }
}

void FleetApi::markRead(const QString& id)
{
    if (!authenticator_ || !authenticator_->linked()) {
        m_fleetThreads->clear();
        return;
    }

    QString url = ("https://api.twitter.com/fleets/v1/mark_read");

    CurlEasy* reply = requestor->post(url, QList<O0RequestParameter>(), QString("{\"fleet_ids\":[\"" + id + "\"]}").toUtf8());

    connect(reply, SIGNAL(done(CURLcode)), reply, SLOT(deleteLater()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void FleetApi::onFleetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();
    if(content.isEmpty())
        return;

    m_fleetThreads->clear();
    m_fleetsParsed = 0;

    m_threads = content["hydrated_threads"].toList();

    m_allRead = true;
    for(int i = 0; i<m_threads.size(); i++){
        requestUserInfo(i);
        m_allRead &= m_threads[i].toMap()["fully_read"].toBool();
    }

    emit allReadChanged();
    reply->deleteLater();
}

void FleetApi::requestUserInfo(int thread_index){
    if (!authenticator_ || !authenticator_->linked()) {
        m_fleetThreads->clear();
        return;
    }

    QString url = ("https://api.twitter.com/graphql/OFPo_sGBE3tETwMWzjagLQ/UserByIdQuery");
    QList<O0RequestParameter> par;
    QString query("{\"rest_id\":\"" + m_threads[thread_index].toMap()["user_id_str"].toString() + "\",\"includeTranslatableProfile\":false}");
    par.append(O0RequestParameter("variables", query.toUtf8()));

    CurlEasy* reply = requestor->get(url, par);
    reply->setPrivateData(thread_index);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onUserInfo()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void FleetApi::onUserInfo()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    int index = reply->privateData().toInt();

    QVariantMap thread = m_threads[index].toMap();
    thread["user"] = content["data"].toMap()["user"].toMap()["legacy"].toMap();
    m_threads[index] = thread;
    m_fleetsParsed++;

    if(m_fleetsParsed == m_threads.size()){
        m_fleetThreads->append(m_threads);
//        m_threads.clear();
        emit fleetsLoaded();
    }

    reply->deleteLater();
}

void FleetApi::parseFleets(const QVariantList& threads){
    for(int i = 0; i<threads.size(); i++){
        m_fleetThreads->append(threads[i]);
    }
}
