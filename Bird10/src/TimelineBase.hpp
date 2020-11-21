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

#ifndef TIMELINEBASE_HPP_
#define TIMELINEBASE_HPP_

#include "TwitterApiBase.hpp"
#include <src/QJson4/QJsonObject.h>

class TimelineBase: public TwitterApiBase
{
    Q_OBJECT

public:
    TimelineBase(QObject *parent = 0);
    virtual ~TimelineBase();

protected:

    QVariantMap parseTweet(QVariantMap tweet);
    QJsonObject realTweet(QJsonObject tweet);
    QVariantMap realTweet(QVariantMap tweet);
    QVariantMap realTweetV2(const QVariantMap& tweet, const QVariantMap& tweets, const QVariantMap& users);
    QVariantMap parseTweetV2(QVariantMap tweet, const QVariantMap& tweets, const QVariantMap& users);
};

#endif /* TIMELINEBASE_HPP_ */
