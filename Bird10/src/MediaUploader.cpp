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

#include <src/MediaUploader.hpp>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonObject.h>
#include <QDebug>
#include <QTimer>

MediaUploader::MediaUploader(O1Twitter* authenticator, QObject* parent) : TwitterApiBase(parent)
{
    setAuthenticator(authenticator);
    m_index = 0;
}

void MediaUploader::uploadVideo(const QString& path)
{
    m_mediaType = VIDEO;

    if (!authenticator_ || !authenticator_->linked()) {
        emit error("Not logged in");
        return;
    }

    m_currentFile.setFileName(path);
    m_currentFile.open(QIODevice::ReadOnly);
    if(!m_currentFile.isOpen()){
        emit error("Cannot open media");
        return;
    }
    if(m_currentFile.size() > 15 * 1000 * 1000){
        emit error("Video is too large");
        return;
    }

    QString url = ("https://upload.twitter.com/1.1/media/upload.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("command", "INIT"));
    par.append(O0RequestParameter("total_bytes", QByteArray::number(m_currentFile.size())));
    par.append(O0RequestParameter("media_type", "video/mp4"));
    par.append(O0RequestParameter("media_category", "tweet_video"));

    qDebug()<<"MediaUploader::uploadVideo: total_bytes: "<<QByteArray::number(m_currentFile.size());

    CurlEasy* reply = requestor->post(url, par);


    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(append()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}

void MediaUploader::uploadPictures(const QList<QString> paths)
{
}

void MediaUploader::append()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    if(content.contains("media_id_string")) // INIT replies with the media id, subsequent calls to APPEND don't
        m_id = content["media_id_string"].toString();

    QByteArray chunk = m_currentFile.read(1*1000*1000);
    qDebug()<<"MediaUploader::append: chunk size: "<<chunk.size();

    QString url = ("https://upload.twitter.com/1.1/media/upload.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("command", "APPEND"));
    par.append(O0RequestParameter("media_id", m_id.toUtf8()));
    par.append(O0RequestParameter("segment_index", QByteArray::number(m_index)));
    par.append(O0RequestParameter("media_data", chunk.toBase64()));
    m_index++;

    reply->deleteLater();
    reply = requestor->post(url, par);

    if(chunk.size() < 1*1000*1000)
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(finalize()));
    else
        connect(reply, SIGNAL(done(CURLcode)), this, SLOT(append()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}


void MediaUploader::finalize(){
    sender()->deleteLater();
    m_currentFile.close();

    qDebug()<<"MediaUploader::finalize: file closed";

    QString url = ("https://upload.twitter.com/1.1/media/upload.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("command", "FINALIZE"));
    par.append(O0RequestParameter("media_id", m_id.toUtf8()));

    CurlEasy* reply = requestor->post(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(checkStatus()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);

}

void MediaUploader::checkStatus()
{
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
    QVariantMap content = jsonResponse.object().toVariantMap();

    qDebug()<<content;

    if(content.keys().contains("processing_info")){
        QString state = content["processing_info"].toMap()["state"].toString();
        if(state == "failed"){
            emit error("Media upload failed: "+ content["processing_info"].toMap()["error"].toMap()["message"].toString());
            reply->deleteLater();
            return;
        }
        else if(state != "succeeded"){
            int check_after_secs = content["processing_info"].toMap()["check_after_secs"].toInt();
            if(check_after_secs > 0)
                QTimer::singleShot(check_after_secs*1000, this, SLOT(status()));
                reply->deleteLater();
                return;
        }
    }

    m_mediaIds << content["media_id_string"].toString();
    m_index = 0;

    emit uploadComplete(m_mediaIds.join(","));

    reply->deleteLater();
}

void MediaUploader::status()
{
    QString url = ("https://upload.twitter.com/1.1/media/upload.json");
    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("command", "STATUS"));
    par.append(O0RequestParameter("media_id", m_id.toUtf8()));

    CurlEasy* reply = requestor->get(url, par);

    connect(reply, SIGNAL(done(CURLcode)), this, SLOT(checkStatus()));
    connect(reply, SIGNAL(error(CURLcode)), this, SLOT(onRequestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(reply, "perform", Qt::QueuedConnection);
}
