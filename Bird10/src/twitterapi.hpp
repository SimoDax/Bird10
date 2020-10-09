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

#ifndef TWITTERAPI_H
#define TWITTERAPI_H

#include <QAbstractListModel>
#include <QObject>
#include <QNetworkAccessManager>
#include <QThread>
#include <bb/cascades/ArrayDataModel>
#include <src/o2/oXtwitter.h>
#include <src/o2/o1requestor.h>
#include <src/QJson4/QJsonObject.h>
#include <src/TimelineBase.hpp>
#include <src/TimelineDataModel.hpp>


/// Mini Twitter API
class TwitterApi: public TimelineBase {
    Q_OBJECT

public:
    /// List of tweets
    Q_PROPERTY(TimelineDataModel *tweetModel READ tweetModel WRITE setTweetModel NOTIFY tweetModelChanged)
    TimelineDataModel *tweetModel() ;
    void setTweetModel(TimelineDataModel *tweetModel);


    explicit TwitterApi(QObject *parent = 0);
    virtual ~TwitterApi();

    Q_INVOKABLE inline void clearTweetModel(){  tweetModel_->clear();}

public slots:
    Q_INVOKABLE virtual void requestTweets(QString max_id = QString(), QString since_id = QString());
    Q_INVOKABLE virtual void requestFavoriteTweets(QString max_id = QString(), QString since_id = QString());
//    Q_INVOKABLE virtual void requestProfileTweets(QString max_id = QString(), QString since_id = QString());
    Q_INVOKABLE void requestOlderTweets();
    Q_INVOKABLE void requestOlderFavoriteTweets();
//    Q_INVOKABLE void requestOlderProfileTweets();
    Q_INVOKABLE void requestLatestTweets();

    Q_INVOKABLE void favorite(QString id, bool state);
    Q_INVOKABLE void destroyTweet(const QString& id);

signals:
    void tweetModelChanged();
    void userNotFoundError();

protected:
    TimelineDataModel *tweetModel_;

protected slots:
    void tweetsReceived();
    void olderTweetsReceived();
    void latestTweetsReceived();
    void requestFailed(CURLcode error);
    void retweeted();
    void unretweeted();
    void favorited();
    void retweet(QString id, bool state);
    void onDestroyTweet();

protected:
    void appendTweets(CurlEasy* reply);
};

#endif // TWITTERAPI_H
