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

#include <src/QJson4/QJsonArray.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonObject.h>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/ItemGrouping>
#include <QMetaObject>
#include <QFile>
#include <string>

#include "src/o2/o1requestor.h"
#include "twitterapi.hpp"
#include <src/RetweetDialog.hpp>
#include <src/TimelineBase.hpp>


TwitterApi::TwitterApi(QObject *parent): TimelineBase(parent) {
    tweetModel_ = new TimelineDataModel(this);
    //requestor = new O1Requestor(authenticator_, this);
    //thread.start();
}

TwitterApi::~TwitterApi() {
    //thread.quit();
    //thread.wait();
    //tweetModel_->deleteLater();
    requestor->deleteLater();
}

TimelineDataModel *TwitterApi::tweetModel() {
    return tweetModel_;
}

void TwitterApi::setTweetModel(TimelineDataModel* tweetModel)
{
    TimelineDataModel* oldModel = tweetModel_;
    tweetModel_ = tweetModel;

    emit tweetModelChanged();

    if(oldModel)
        oldModel->deleteLater();
}

void TwitterApi::requestTweets(QString max_id, QString since_id) {
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }
    //CurlEasy * requestor = new CurlEasy(this);

    //O1Requestor *requestor = new O1Requestor(authenticator_, this);
    //requestor->moveToThread(thread);
    QString url = ("https://api.twitter.com/1.1/statuses/home_timeline.json?tweet_mode=extended&count=40");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "40"));

    if(!max_id.isEmpty()){
        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
    }
    if(!since_id.isEmpty()){
        url += "&since_id=" + QString("%1").arg(since_id.toLongLong());
        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(!max_id.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(olderTweetsReceived()));
    else if(!since_id.isEmpty())
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(latestTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(tweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::requestFavoriteTweets(QString max_id, QString since_id) {
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/1.1/favorites/list.json?tweet_mode=extended&count=40");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "40"));

    if(!max_id.isEmpty()){
        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
    }
    if(!since_id.isEmpty()){
        url += "&since_id=" + QString("%1").arg(since_id.toLongLong());
        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()).toUtf8()));
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

//void TwitterApi::requestProfileTweets(QString max_id, QString since_id) {
//    if (!authenticator_ || !authenticator_->linked()) {
//        tweetModel_->clear();
//        emit tweetModelChanged();
//        return;
//    }
//
//    QString url = ("https://api.twitter.com/1.1/statuses/user_timeline.json?tweet_mode=extended&count=40");
//    QList<O0RequestParameter> par;
//    par.append(O0RequestParameter("tweet_mode", "extended"));
//    par.append(O0RequestParameter("count", "40"));
//
//    if(!max_id.isEmpty()){
//        url += "&max_id=" + QString("%1").arg(max_id.toLongLong()-1);
//        par.append(O0RequestParameter("max_id", QString("%1").arg(max_id.toLongLong()-1).toUtf8()));
//    }
//    if(!since_id.isEmpty()){
//        url += "&since_id=" + QString("%1").arg(since_id.toLongLong()-1);
//        par.append(O0RequestParameter("since_id", QString("%1").arg(since_id.toLongLong()-1).toUtf8()));
//    }
//
//
//    CurlEasy* reply = requestor->get(url, par);
//
//    if(!max_id.isEmpty())
//        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(olderTweetsReceived()));
//    else
//        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(tweetsReceived()));
//    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));
//
//    //Since the object lives on a different thread its function calls need to be queued
//    //This will trigger the perform method without making an unnecessary signal-slot connection
//    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
//}


void TwitterApi::requestOlderTweets(){
    QString last_tweet_id = tweetModel_->value(tweetModel_->size()-1-1).toMap()["id_str"].toString();

    requestTweets(last_tweet_id);
}

void TwitterApi::latestTweets(){
    QString top_tweet_id = tweetModel_->value(0).toMap()["id_str"].toString();

    requestTweets(QString(), top_tweet_id);
}

void TwitterApi::requestOlderFavoriteTweets(){
    QString last_tweet_id = tweetModel_->value(tweetModel_->size()-1-1).toMap()["id_str"].toString();

    requestFavoriteTweets(last_tweet_id);
}

//void TwitterApi::requestOlderProfileTweets(){
//    QString last_tweet_id = tweetModel_->value(tweetModel_->size()-1-1).toMap()["id_str"].toString();
//
//    requestProfileTweets(last_tweet_id);
//}

void TwitterApi::tweetsReceived() {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonArray jsonArray = QJsonDocument::fromJson(reply->data()).array();

    qDebug()<<"TwitterApi::tweetsReceived";

    tweetModel_->clear();
    appendTweets(reply);

    reply->deleteLater();
}

void TwitterApi::olderTweetsReceived() {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    tweetModel_->removeAt(tweetModel_->size()-1);       // Remove loading element
    appendTweets(reply);

    reply->deleteLater();
}

void TwitterApi::latestTweetsReceived()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    tweetModel_->refreshElapsedTime();

    int count=0;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonArray jsonArray = jsonResponse.array();
    qDebug()<<"TwitterApi::latestTweetsReceived: "<<jsonArray.size()<<" new tweets";
    if(!jsonArray.isEmpty()){
        foreach (const QJsonValue &v, jsonArray) {
            QVariantMap item = v.toObject().toVariantMap();
            QVariantMap tweet = TimelineBase::realTweet(item);

            tweetModel_->insert(count, parseTweet(tweet));
            count++;
        }

        emit tweetModelChanged();
    }

    reply->deleteLater();
}

void TwitterApi::appendTweets(CurlEasy* reply){

//    #ifdef QT_DEBUG
//        QFile file("./data/tweets.json");
//        file.open(QIODevice::WriteOnly);
//        if(file.isOpen()){
//            file.write(reply->data());
//            file.close();
//        }
//    #endif

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonArray jsonArray = jsonResponse.array();
    if(!jsonArray.isEmpty()){
        foreach (const QJsonValue &v, jsonArray) {
            QVariantMap item = v.toObject().toVariantMap();
            QVariantMap tweet = TimelineBase::realTweet(item);

            tweetModel_->append(parseTweet(tweet));
        }
        tweetModel_->append(QVariant());    //Empty QVariant for loading element

        emit tweetModelChanged();
    }

}


void TwitterApi::requestFailed(CURLcode error) {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    long code = reply->get<long>(CURLINFO_RESPONSE_CODE);

    if(error == CURLE_OK)
        if(code == 429)
            emit this->error("Too many requests in a 15-minutes window. Please try again later");
        else if (code == 404)
            emit userNotFoundError();
        else
            emit this->error("Twitter error: " + code);
    else
        emit networkError();
    qWarning() << "TwitterApi::requestFailed:" << (int)error << code << reply->data();

    reply->deleteLater();
}



void TwitterApi::retweet(QString id, bool state){
    QString url;

    if(!state)
        url = ("https://api.twitter.com/1.1/statuses/retweet/" + id + ".json");
    else
        url = ("https://api.twitter.com/1.1/statuses/unretweet/" + id + ".json");

    CurlEasy * reply = requestor->post(url, QList<O0RequestParameter>(), QByteArray());
    if(!state)
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(retweeted()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(unretweeted()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::retweeted(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonObject tweet = realTweet(jsonResponse.object());

    QString id = tweet["id_str"].toString();

    for(int i = 0; i<tweetModel_->size(); i++){
        QVariantMap item = tweetModel_->value(i).toMap();

        if(tweet.keys().contains("retweeted_status"))
            item = item["retweeted_status"].toMap();

        if(item["id_str"].toString() == id){
            item["retweeted"] = tweet["retweeted"].toBool();
            tweetModel_->replace(i, QVariant(QVariantMap(item.toStdMap())));
            //emit tweetModelChanged();
        }
    }
    reply->deleteLater();
}

void TwitterApi::unretweeted(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonObject tweet = realTweet(jsonResponse.object());

    QString id = tweet["id_str"].toString();

    for(int i = 0; i<tweetModel_->size(); i++){
        QVariantMap item = tweetModel_->value(i).toMap();

        if(tweet.keys().contains("retweeted_status"))
            item = item["retweeted_status"].toMap();

        if(item["id_str"].toString() == id){
            item["retweeted"] = false;
            tweetModel_->replace(i, QVariant(QVariantMap(item.toStdMap())));
            //emit tweetModelChanged();
        }
    }
    reply->deleteLater();
}

void TwitterApi::favorite(QString id, bool state){
    QString url;

    if(!state)
        url = ("https://api.twitter.com/1.1/favorites/create.json");
    else
        url = ("https://api.twitter.com/1.1/favorites/destroy.json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("id", id.toLatin1()));

    CurlEasy * reply = requestor->post(url, par, QByteArray());
    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(favorited()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::favorited(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonObject tweet = realTweet(jsonResponse.object());

    QString id = tweet["id_str"].toString();
    //qDebug()<<"TwitterApi::favorited "<<tweet["id_str"] << " " << tweet["favorited"];

    for(int i = 0; i<tweetModel_->size(); i++){
       QVariantMap item = tweetModel_->value(i).toMap();

       if(item.keys().contains("retweeted_status"))
           item = item["retweeted_status"].toMap();

       if(item["id_str"].toString() == id){
           item["favorited"] = tweet["favorited"].toBool();
           qDebug()<<tweet["favorite_count"].toString();
           tweetModel_->replace(i, QVariant(QVariantMap(item.toStdMap())));
           //emit tweetModelChanged();
       }
    }
    //emit tweetModelChanged();
    reply->deleteLater();
}

void TwitterApi::destroyTweet(const QString& id)
{
    QString url = ("https://api.twitter.com/1.1/statuses/destroy/" + id + ".json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("id", id.toLatin1()));

    CurlEasy * reply = requestor->post(url, par, QByteArray());
    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onDestroyTweet()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::onDestroyTweet(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap tweet = jsonResponse.object().toVariantMap();

    tweetModel_->removeById(tweet["id_str"].toString());

    reply->deleteLater();
}
