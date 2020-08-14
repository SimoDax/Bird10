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

#include <src/TimelineBase.hpp>
#include <QDateTime>
#include <QDebug>

TimelineBase::TimelineBase(QObject *parent) : TwitterApiBase(parent)
{
    // TODO Auto-generated constructor stub

}

TimelineBase::~TimelineBase()
{
}


QVariantMap TimelineBase::realTweet(QVariantMap tweet)
{
    if(tweet.keys().contains("retweeted_status")){
        QVariantMap real_tweet = tweet["retweeted_status"].toMap();
        //real_tweet["id_str"] = tweet["id_str"];     //omg haxor
        real_tweet["rt_flag"] = true;
        real_tweet["rt_name"] = tweet["user"].toMap()["name"];
        real_tweet["rt_screen_name"] = tweet["user"].toMap()["screen_name"];
        real_tweet["rt_id"] = tweet["id_str"];
        return real_tweet;
    }
    else{
        tweet["rt_flag"] = false;
        return tweet;
    }
}

QJsonObject TimelineBase::realTweet(QJsonObject tweet){
    if(tweet.keys().contains("retweeted_status"))
            return tweet["retweeted_status"].toObject();
        else
            return tweet;
}

QVariantMap TimelineBase::parseTweet(QVariantMap tweet)
{
    // Parse media type (if any)

    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "photo")
        tweet["image_flag"] = true;
    else
        tweet["image_flag"] = false;

    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "video")
        tweet["video_flag"] = true;
    else
        tweet["video_flag"] = false;

    if(tweet.keys().contains("extended_entities") && tweet["extended_entities"].toMap()["media"].toList()[0].toMap()["type"].toString() == "animated_gif")
        tweet["gif_flag"] = true;
    else
        tweet["gif_flag"] = false;


    // Rewrite tweet, use utf-32 to count emoji only once
    std::wstring text = tweet["full_text"].toString().toStdWString();

    if(tweet.keys().contains("display_text_range")){
        // Crop quoted tweet link
        text.erase(tweet["display_text_range"].toList()[1].toInt());
    }

    QVariantList urls = tweet["entities"].toMap()["urls"].toList();

    int offset = 0;
    for(int i = 0; i<urls.size(); i++){
        int beginIndex = urls[i].toMap()["indices"].toList()[0].toInt();
        if(beginIndex+offset >= text.size())   //link already cropped out by display_text_range
            continue;
        int endIndex = urls[i].toMap()["indices"].toList()[1].toInt();

        //QString htmlUrl = "<a href = \"" + urls[i].toMap()["url"].toString() + "\">"+ urls[i].toMap()["expanded_url"].toString() + "</a>";
        QString htmlUrl = urls[i].toMap()["expanded_url"].toString();   // coupled with content.flags in textbox

        text.replace(beginIndex+offset, endIndex-beginIndex, htmlUrl.toStdWString());
        //text.remove(beginIndex+offset, endIndex-beginIndex);
        //text.insert(beginIndex+offset, htmlUrl);

        offset += htmlUrl.length() - (endIndex - beginIndex);   //keeps track of indexes changes due to html insertion

    }
    tweet["full_text"] = QString::fromStdWString(text);

    if(tweet.keys().contains("limited_actions") && tweet["limited_actions"].toString() == "limited_replies")
        tweet["limited_replies"] = true;


    // ISO date parsing follows -- DO NOT TOUCH!

    QDateTime dt;
    QStringList l = tweet["created_at"].toString().split(" ");
    QString iso;

//    qDebug()<<"ISO list size: "<<l.size();
    if(l.size() >= 6){

        iso.append(l[5] + "-");

        QStringList months;
        months<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";

        int index = months.indexOf(l[1], Qt::CaseInsensitive)+1;
        QString month = QString::number(index);
        if(month.size() == 1)
            month.prepend("0");

        iso.append(month + "-");
        iso.append(l[2]);
        iso.append("T");
        iso.append(l[3]);

        if(l[4] == "+0000")
            iso.append("Z");
        else
            iso.append(l[4].mid(0,2) + ":" + l[4].mid(3,4));

        dt = QDateTime::fromString(iso, Qt::ISODate);
        QDateTime current = QDateTime::currentDateTime();

        int deltaDays = dt.daysTo(current);
        if(deltaDays == 0 || deltaDays == 1){
            int deltaHours = dt.secsTo(current)/3600;
//            qDebug()<<deltaHours;

            if(deltaHours == 0){
                int deltaMinutes = dt.secsTo(current)/60;
                if(deltaMinutes == 0)
                    tweet["time"] = QString::number(dt.secsTo(current)) + "s";
                else
                    tweet["time"] = QString::number(deltaMinutes) + "min";
            }
            else
                tweet["time"] = QString::number(deltaHours) + "h";
        }
        else if (deltaDays <= 7)
            tweet["time"] = QString::number(deltaDays) + "d";
        else if(dt.date().year() == current.date().year())
            tweet["time"] = dt.toString("d MMM").toLower();
        else
            tweet["time"] = dt.toString("d MMM yyyy").toLower();

        tweet["full_time"] = dt.toString("h:mm AP " + QString::fromUtf8("\u00B7") +" d MMM yyyy").toLower();
    }


    return tweet;
}

QVariantMap TimelineBase::parseTweetV2(QVariantMap tweet, const QVariantMap& tweets, const QVariantMap& users){

    tweet = parseTweet(tweet);

    tweet["user"] = users[tweet["user_id_str"].toString()];

    if(tweet["is_quote_status"].toBool() == true && tweet.keys().contains("quoted_status") == false){
        QString quoteId = tweet["quoted_status_id_str"].toString();

        QVariantMap quoteTweet = parseTweet((tweets[quoteId].toMap()));

        quoteTweet["user"] = users[quoteTweet["user_id_str"].toString()];

        tweet["quoted_status"] = quoteTweet;
    }

    return tweet;

}
