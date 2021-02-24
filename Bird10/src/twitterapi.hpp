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

protected: enum Direction{
        NONE,
        TOP,
        BOTTOM
    };

public:
    /// List of tweets
    Q_PROPERTY(TimelineDataModel *tweetModel READ tweetModel WRITE setTweetModel NOTIFY tweetModelChanged)
    TimelineDataModel *tweetModel() ;
    void setTweetModel(TimelineDataModel *tweetModel);


    explicit TwitterApi(QObject *parent = 0);
    virtual ~TwitterApi();

    Q_INVOKABLE inline void clearTweetModel(){  tweetModel_->clear();}
    Q_INVOKABLE void votePoll(const QString& card_uri, const QString& tweet_id, int selected_choice, int choice_count);

public slots:
    Q_INVOKABLE virtual void requestTweets(Direction dir = NONE);
    Q_INVOKABLE virtual void requestFavoriteTweets(Direction dir = NONE);
    Q_INVOKABLE void requestOlderTweets();
    Q_INVOKABLE void requestOlderFavoriteTweets();
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
    void onVotePoll();

protected:
    void processTimeline(CurlEasy* reply, Direction mode = NONE);
    int insertTweet(const QVariantMap& tweet, const QVariantMap& tweets, const QVariantMap& users, int position = -1, bool dividerVisible = true);
    int insertTweetFromConversation(const QVariantMap& tweet, const QVariantMap& tweets, const QVariantMap& users, int position = -1);
    void terminateTimeline(QVariantMap instruction);
};

#endif // TWITTERAPI_H
