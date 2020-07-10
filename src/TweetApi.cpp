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

#include <QFile>
#include <src/QJson4/QJsonObject.h>
#include <src/QJson4/QJsonDocument.h>
#include <QDebug>

#include <src/TweetApi.hpp>

TweetApi::TweetApi(QObject *parent) : TwitterApiBase(parent){

}

void TweetApi::tweet(QString status, QString in_reply_to_status_id, QString attachment_url){
    QString url = ("https://api.twitter.com/1.1/statuses/update.json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("status", status.toUtf8()));
    if(!in_reply_to_status_id.isEmpty()){
        par.append(O0RequestParameter("in_reply_to_status_id", in_reply_to_status_id.toLatin1()));
        par.append(O0RequestParameter("auto_populate_reply_metadata", "true"));
    }
    if(!attachment_url.isEmpty())
        par.append(O0RequestParameter("attachment_url", attachment_url.toLatin1()));

    //par.append(O0RequestParameter("trim_user", "true"));
//    QString par;
//    par.append("status=" + status.toLatin1());
//    par.append("&trim_user=true");

    CurlEasy * reply = requestor->post(url, par);
    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onTweeted()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void TweetApi::onTweeted(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    emit tweeted();

    qDebug()<<"TweetApi::tweeted "<<reply->data();
    reply->deleteLater();
}

void TweetApi::imageTweet(QString status, QVariantList images, QString in_reply_to_status_id, QString attachment_url){
    imageStatus = status;
    image_reply_status_id = in_reply_to_status_id;
    image_attachment_url = attachment_url;
    qDebug()<<"TweetApi::imageTweet, status length: "<<status.size();

    for(int i = 0; i<images.size(); i++){
       if(images[i].toString().isEmpty())
           continue;

       QString url = ("https://upload.twitter.com/1.1/media/upload.json");

       //TODO check for file size < 5MB
       QFile image(images[i].toString());
       image.open(QIODevice::ReadOnly);
       if(!image.isOpen())
           continue;

       QList<O0RequestParameter> par;
       QByteArray imageBytes  = image.readAll();
       //qDebug()<<imageBytes.size();
       par.append(O0RequestParameter("media_data", imageBytes.toBase64()));

       image.close();
       mediaLeft++;

       CurlEasy * reply = requestor->post(url, par, QByteArray());

       qDebug()<<connect(reply, SIGNAL(done(CURLcode)), this, SLOT(imagePosted()), Qt::QueuedConnection); //Force queued so that slots won't be called simultaneously
       qDebug()<<connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));
       QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

    }
}

void TweetApi::imagePosted(){
    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());

    qDebug()<<reply->data();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    media_ids << jsonResponse.object().value("media_id_string").toString();

    mediaLeft--;
    if(!mediaLeft)
       postImageTweet();

    reply->deleteLater();
}

void TweetApi::postImageTweet(){
    QString url = ("https://api.twitter.com/1.1/statuses/update.json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("status", imageStatus.toUtf8()));
    if(!image_reply_status_id.isEmpty()){
            par.append(O0RequestParameter("in_reply_to_status_id", image_reply_status_id.toLatin1()));
            par.append(O0RequestParameter("auto_populate_reply_metadata", "true"));
            image_reply_status_id.clear();
    }
    if(!image_attachment_url.isEmpty()){
            par.append(O0RequestParameter("attachment_url", image_attachment_url.toLatin1()));
            image_attachment_url.clear();
    }

    qDebug()<<media_ids;
    if(media_ids.size()){
        QString media_id = media_ids[0].toString();
        for(int i = 1; i<media_ids.size(); i++)
            media_id += "," + media_ids[i].toString();

        par.append(O0RequestParameter("media_ids", media_id.toLatin1()));
    }

    CurlEasy * reply = requestor->post(url, par, QByteArray());
    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onTweeted()));
    bool ok = connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));
    Q_ASSERT(ok);
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}


