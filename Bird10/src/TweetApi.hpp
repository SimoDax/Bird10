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

#ifndef TWEETAPI_HPP_
#define TWEETAPI_HPP_

#include "TwitterApiBase.hpp"
#include <QObject>

class TweetApi: public TwitterApiBase{
    Q_OBJECT

public:
    TweetApi(QObject *parent = 0);

    Q_INVOKABLE void tweet(QString status, QString in_reply_to_status_id, QString attachment_url);
    Q_INVOKABLE void imageTweet(QString status, QVariantList images, QString in_reply_to_status_id, QString attachment_url);
    Q_INVOKABLE void videoTweet(const QString& status, const QString& video, const QString& in_reply_to_status_id, const QString& attachment_url);

signals:
    void tweeted();
    void mediaSizeError();

protected slots:
    void onTweeted();
    void postMediaTweet(const QString& media_ids);

protected:
    QString m_status, reply_status_id, m_attachment_url;
};

#endif /* TWEETAPI_HPP_ */
