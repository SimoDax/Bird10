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

#ifndef PROFILEAPI_HPP_
#define PROFILEAPI_HPP_

#include "TwitterApiBase.hpp"
#include <src/TimelineBase.hpp>
#include <bb/cascades/ArrayDataModel>
#include <src/QJson4/QJsonArray.h>
#include <src/TimelineDataModel.hpp>

class ProfileApi: public TimelineBase
{
    Q_OBJECT
public:
    ProfileApi(QObject *parent = 0);

    /// List of tweets
    Q_PROPERTY(TimelineDataModel *tweetModel READ tweetModel NOTIFY tweetModelChanged)
    inline TimelineDataModel *tweetModel() { return tweetModel_; } ;

    /// List of tweets with replies
    Q_PROPERTY(TimelineDataModel *fullTweetModel READ fullTweetModel NOTIFY fullTweetModelChanged)
    inline TimelineDataModel *fullTweetModel() { return fullTweetModel_; } ;

    /// List of  media tweets
    Q_PROPERTY(TimelineDataModel *mediaTweetModel READ mediaTweetModel NOTIFY mediaTweetModelChanged)
    inline TimelineDataModel *mediaTweetModel() { return mediaTweetModel_; } ;

    /// Small list of tweets
    Q_PROPERTY(TimelineDataModel *previewTweetModel READ previewTweetModel NOTIFY previewTweetModelChanged)
    inline TimelineDataModel *previewTweetModel() { return previewTweetModel_; } ;

    /// Profile data
    Q_PROPERTY(TimelineDataModel *userModel READ userModel NOTIFY userModelChanged)
    inline TimelineDataModel *userModel(){ return userModel_; } ;

    /// Screen name
    Q_PROPERTY(QString screenName READ screenName WRITE setScreenName)
    inline QString screenName(){ return m_screen_name; } ;
    inline void setScreenName(QString screenName){ m_screen_name =  screenName; } ;

    /// Following
    Q_PROPERTY(bool following READ following NOTIFY followingChanged)
    inline bool following(){ return m_following; } ;


public slots:
    Q_INVOKABLE void requestUserTweets(QString max_id = QString(), QString since_id = QString());
    Q_INVOKABLE void requestOlderUserTweets();
    Q_INVOKABLE void requestFullUserTweets(QString max_id = QString(), QString since_id = QString());
    Q_INVOKABLE void requestOlderFullUserTweets();
    Q_INVOKABLE void requestMediaUserTweets(QString cursor = QString());
    Q_INVOKABLE void requestOlderMediaUserTweets();
    Q_INVOKABLE void followUser(const QString& id, bool following);

signals:
    void tweetModelChanged();
    void fullTweetModelChanged();
    void mediaTweetModelChanged();
    void previewTweetModelChanged();
    void userModelChanged();
    void userNotFoundError();
    void followed();
    void unfollowed();
    void followingChanged();

protected slots:
    void onTweetsReceived();
    void onOlderTweetsReceived();
    void onFullTweetsReceived();
    void onOlderFullTweetsReceived();
    void onMediaTweetsReceived();
    void onOlderMediaTweetsReceived();
    void onFollowed();

protected:
//    QVariantMap realTweet(QVariantMap tweet);
//    //QJsonObject realTweet(QJsonObject tweet);
//    QVariantMap parseTweet(QVariantMap tweet);
    void appendTweets(CurlEasy* reply, TimelineDataModel * dataModel);
    void insertMediaTweets(CurlEasy* reply, int offset = 0);
    void requestUserData();
    void populateUserData(const QJsonArray& tweets);

protected:
    TimelineDataModel *previewTweetModel_, *tweetModel_, *mediaTweetModel_, *fullTweetModel_, *userModel_;
    QString m_screen_name, m_id_str, m_mediaCursor;
    bool m_following;
};

#endif /* PROFILEAPI_HPP_ */
