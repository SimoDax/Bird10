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
#include <src/MediaUploader.hpp>

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
    m_status = status;
    reply_status_id = in_reply_to_status_id;
    m_attachment_url = attachment_url;
    qDebug()<<"TweetApi::imageTweet, status length: "<<status.size();

    MediaUploader* mu = new MediaUploader(authenticator_, this);
    connect(mu, SIGNAL(uploadComplete(QString)), this, SLOT(postMediaTweet(QString)));
    connect(mu, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    mu->uploadPictures(QVariant(images).toStringList());
}

void TweetApi::videoTweet(const QString& status, const QString& video, const QString& in_reply_to_status_id, const QString& attachment_url)
{
    m_status = status;
    reply_status_id = in_reply_to_status_id;
    m_attachment_url = attachment_url;
    MediaUploader* mu = new MediaUploader(authenticator_, this);
    connect(mu, SIGNAL(uploadComplete(QString)), this, SLOT(postMediaTweet(QString)));
    connect(mu, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    mu->uploadVideo(video);
}

void TweetApi::postMediaTweet(const QString& media_ids)
{
    QString url = ("https://api.twitter.com/1.1/statuses/update.json");

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("status", m_status.toUtf8()));
    if(!reply_status_id.isEmpty()){
            par.append(O0RequestParameter("in_reply_to_status_id", reply_status_id.toLatin1()));
            par.append(O0RequestParameter("auto_populate_reply_metadata", "true"));
            reply_status_id.clear();
    }
    if(!m_attachment_url.isEmpty()){
            par.append(O0RequestParameter("attachment_url", m_attachment_url.toLatin1()));
            m_attachment_url.clear();
    }

    par.append(O0RequestParameter("media_ids", media_ids.toUtf8()));

    CurlEasy * reply = requestor->post(url, par, QByteArray());
    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(onTweeted()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

    sender()->deleteLater();    //clean up mediauploader
}
