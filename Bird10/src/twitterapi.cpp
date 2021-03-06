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

void TwitterApi::requestTweets(Direction dir) {
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/timeline/home_latest.json");
    QList<O0RequestParameter> par = basicGetParameters();
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "40"));

    if(dir == BOTTOM){
        par.append(O0RequestParameter("cursor", QString("%1").arg(tweetModel_->cursorBottom()).toUtf8()));
    }
    if(dir == TOP){
        par.append(O0RequestParameter("cursor", QString("%1").arg(tweetModel_->cursorTop()).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);

    if(dir == BOTTOM)
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(olderTweetsReceived()));
    else if(dir == TOP)
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(latestTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(tweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::requestFavoriteTweets(Direction dir) {
    if (!authenticator_ || !authenticator_->linked()) {
        tweetModel_->clear();
        emit tweetModelChanged();
        return;
    }

    QString url = ("https://api.twitter.com/2/timeline/favorites/" + authenticator_->extraTokens()["user_id"].toString() +".json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("count", "40"));
    par.append(O0RequestParameter("sorted_by_time", "true"));

    if(dir == BOTTOM){
        par.append(O0RequestParameter("cursor", QString("%1").arg(tweetModel_->cursorBottom()).toUtf8()));
    }

    CurlEasy* reply = requestor->get(url, par);
    reply->set(CURLOPT_VERBOSE, 1L);

    if(dir == BOTTOM)
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(olderTweetsReceived()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(tweetsReceived()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}


void TwitterApi::requestOlderTweets(){
    requestTweets(BOTTOM);
}

void TwitterApi::requestLatestTweets(){
    requestTweets(TOP);
}

void TwitterApi::requestOlderFavoriteTweets(){
    requestFavoriteTweets(BOTTOM);
}

void TwitterApi::tweetsReceived() {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonArray jsonArray = QJsonDocument::fromJson(reply->data()).array();

    qDebug()<<"TwitterApi::tweetsReceived";

    tweetModel_->clear();
    processTimeline(reply);

    reply->deleteLater();
}

void TwitterApi::olderTweetsReceived() {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    // sometimes if the network is slow the user can trigger a loadOlder request before the previous returns, so more than 1 loading element may be present
    for(int i = tweetModel_->size()-1; i>0; i--){
        if(tweetModel_->value(i).isNull())
            tweetModel_->removeAt(i);       // Remove loading element
        else break;
    }
    processTimeline(reply, BOTTOM);

    reply->deleteLater();
}

void TwitterApi::latestTweetsReceived(){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    tweetModel_->refreshElapsedTime();
    processTimeline(reply, TOP);

    reply->deleteLater();
}

void TwitterApi::processTimeline(CurlEasy* reply, Direction mode){

//    #ifdef QT_DEBUG
//        QFile file("./data/tweets.json");
//        file.open(QIODevice::WriteOnly);
//        if(file.isOpen()){
//            file.write(reply->data());
//            file.close();
//        }
//    #endif

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QJsonObject jsonObject = jsonResponse.object();
    QVariantMap content = jsonResponse.object().toVariantMap();

    QVariantMap users = content["globalObjects"].toMap()["users"].toMap();
    QVariantMap tweets = content["globalObjects"].toMap()["tweets"].toMap();
    QVariantList instructions = content["timeline"].toMap()["instructions"].toList();

    int count=0;    // if mode is TOP we need to insert tweets preserving the order, so we need to insert each one at <count> position and increment count afterwise
    for(int i = 0; i<instructions.size(); i++){
        if(instructions[i].toMap().keys().contains("addEntries")){
            QVariantList entries = instructions[i].toMap()["addEntries"].toMap()["entries"].toList();
//            qDebug()<<instructions[i];

            for(int i = 0; i<entries.size(); i++){
                QStringList ids = entries[i].toMap()["entryId"].toString().split("-");

                if(ids[0].contains("tweet"))
                    count = insertTweet(entries[i].toMap()["content"].toMap(), tweets, users, mode == TOP ? count : -1);
                else if (ids[0].contains("homeConversation"))
                    count = insertTweetFromConversation(entries[i].toMap()["content"].toMap(), tweets, users, mode == TOP ? count : -1);
                else if(mode != TOP && ids[0].contains("cursor") && (ids[1].contains("bottom")))
                    tweetModel_->setCursorBottom(entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString());
                else if(mode != BOTTOM && ids[0].contains("cursor") && (ids[1].contains("top")))
                    tweetModel_->setCursorTop(entries[i].toMap()["content"].toMap()["operation"].toMap()["cursor"].toMap()["value"].toString());
            }
        }
    }
    tweetModel_->append(QVariant());    //Empty QVariant for loading element

    emit tweetModelChanged();
}

int TwitterApi::insertTweet(const QVariantMap& tweet, const QVariantMap& tweets, const QVariantMap& users, int position, bool dividerVisible)
{
    const QString id = tweet["item"].toMap()["content"].toMap()["tweet"].toMap()["id"].toString();

    //TODO: if it's not contained either we're blocked or the account is private, handle this
//    if(!m_tweets.keys().contains(id))
//        return;

    QVariantMap tweetObject = parseTweetV2((tweets[id].toMap()), tweets, users);
    tweetObject["divider_visible_flag"] = dividerVisible;

    if(position < 0)
        tweetModel_->append(tweetObject);
    else
        tweetModel_->insert(position, tweetObject);

    return position+1;  // return new position
}

int TwitterApi::insertTweetFromConversation(const QVariantMap& thread, const QVariantMap& tweets, const QVariantMap& users, int position){
    QVariantList items = thread["timelineModule"].toMap()["items"].toList();

    // check if the mini thread is just tweets of the same author grouped together or if it's actually uninteresting conversation stuff
    bool sameAuthor = true;
    QString author = items[0].toMap()["item"].toMap()["content"].toMap()["tweet"].toMap()["user"].toMap()["id_str"].toString();
    for(int i = 0; i<items.size() && sameAuthor; i++){
        sameAuthor &= author == items[i].toMap()["item"].toMap()["content"].toMap()["tweet"].toMap()["user"].toMap()["id_str"].toString();
    }

    QStringList allTweetIds = thread["timelineModule"].toMap()["metadata"].toMap()["conversationMetadata"].toMap()["allTweetIds"].toStringList();

    int count = position;
    if(sameAuthor){
        bool showThreadInserted = false;
        for(int i = 0; i<items.size(); i++){
            QStringList ids = items[i].toMap()["entryId"].toString().split("-");

            if((ids[0].contains("homeConversation") || ids[0].contains("tweet"))){
                if(ids.last() != allTweetIds[i] && !showThreadInserted && !allTweetIds.isEmpty()){
                    // The id of the current tweet is different from the i-th tweet in the thread -> some have been omissed, insert "Show this thread" element
                    QVariantMap element;
                    element["show_thread_flag"] = true;

                    if(position < 0)
                        tweetModel_->append(element);
                    else{
                        tweetModel_->insert(count, element);
                        count++;
                    }
                    showThreadInserted = true;
                }

                if(position < 0)
                    insertTweet(items[i].toMap(), tweets, users, position, i == items.size()-1); // this will append() the tweet
                else
                    count = insertTweet(items[i].toMap(), tweets, users, count, i == items.size()-1);    // while this will insert it at the correct position
            }
        }
    }
    else{
        for(int i = 0; i<items.size(); i++){
            QStringList ids = items[i].toMap()["entryId"].toString().split("-");

            if((ids[0].contains("homeConversation") || ids[0].contains("tweet")) && items[i].toMap()["dispensable"].toBool() == false){
                if(position < 0)
                    insertTweet(items[i].toMap(), tweets, users, position); // this will append() the tweet
                else
                    count = insertTweet(items[i].toMap(), tweets, users, count);    // while this will insert it at the correct position
            }
        }
    }
    return count;    // return new position
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

void TwitterApi::votePoll(const QString& card_uri, const QString& tweet_id, int selected_choice, int choice_count)
{
    QString url = ("https://caps.twitter.com/v2/capi/passthrough/1");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("twitter:string:card_uri", card_uri.toUtf8()));
    par.append(O0RequestParameter("twitter:long:original_tweet_id", tweet_id.toUtf8()));
    par.append(O0RequestParameter("twitter:string:response_card_name", QString("poll"+ QString::number(choice_count) + "choice_text_only").toUtf8()));
    par.append(O0RequestParameter("twitter:string:cards_platform", "Web-12"));
    par.append(O0RequestParameter("twitter:string:selected_choice", QByteArray::number(selected_choice)));

    CurlEasy * reply = requestor->post(url, par, QByteArray());
    reply->set(CURLOPT_VERBOSE, 1L);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onVotePoll()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TwitterApi::onVotePoll()
{
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap newCard = jsonResponse.object().toVariantMap()["card"].toMap();

    tweetModel_->updateCard(newCard["url"].toString(), newCard);

    reply->deleteLater();
}

void TwitterApi::terminateTimeline(QVariantMap instruction)
{
    QString direction = instruction["terminateTimeline"].toMap()["direction"].toString();
    if(direction == "Top" || direction == "TopAndBottom")
        tweetModel_->setTerminateTop(true);
    if(direction == "Bottom" || direction == "TopAndBottom")
        tweetModel_->setTerminateBottom(true);

    qDebug()<<"TwitterApi::terminateTimeline: "<<instruction;
}
