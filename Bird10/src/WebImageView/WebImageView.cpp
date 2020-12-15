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


#include "WebImageView.h"
#include "../applicationui.hpp"
#include <QNetworkReply>
#include <QNetworkDiskCache>
//#include <QDesktopServices>
#include <bb/cascades/Image>
#include <bb/cascades/InvokeQuery>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
using namespace bb::cascades;
using namespace bb::system;

QNetworkAccessManager * WebImageView::mNetManager = new QNetworkAccessManager();


WebImageView::WebImageView() {

    // Set cache in manager
    if(!mNetManager->cache())
        mNetManager->setCache(ApplicationUI::mNetworkDiskCache);
}

const QUrl& WebImageView::url() const {
    return mUrl;
}

void WebImageView::setUrl(const QUrl& url) {

    if(url.isEmpty())
        return;

    // Either it's already set or is waiting for server reply -> no need to do anything
    if(mUrl == url)
        return;

    // Variables
    //mUrl = url;   //Now set only after the image has been received
    mDesiredUrl = url;
    mDesiredUrl.setScheme("http");


    // Reset the image
    resetImage();

    // Create request
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setUrl(mDesiredUrl);

    // Create reply
    QNetworkReply * reply = mNetManager->get(request);

    // Connect to signals
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));

    //bool ok = QObject::connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslError(const QList<QSslError>&)));
    //Q_ASSERT(ok);

    emit urlChanged();
    emit loadingChanged();
}

void WebImageView::sslError(const QList<QSslError>& errors){
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    for(int i = 0;i<errors.length(); i++){
        qDebug()<<errors[i].errorString();
    }

    reply->ignoreSslErrors();
}


void WebImageView::imageLoaded() {
    // Get reply
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if(reply->url() != mDesiredUrl)
        return;

    if (reply->error() == QNetworkReply::NoError) {
            imageData = reply->readAll();
            setImage(Image(imageData));

            mUrl = reply->url();

            emit imageChanged(Image(imageData));
            emit loadingFinished();
            emit loadingChanged();
    }
    else{
        qDebug()<<"WebImageView error: "<<reply->error();
    }

    // Memory management
    reply->deleteLater();
}

//bool WebImageView::isARedirectedUrl(QNetworkReply *reply) {
//    QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//    return !redirection.isEmpty();
//}
//
//void WebImageView::setURLToRedirectedUrl(QNetworkReply *reply) {
//    QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//    QUrl baseUrl = reply->url();
//    QUrl resolvedUrl = baseUrl.resolved(redirectionUrl);
//
//    setUrl(resolvedUrl.toString());
//}

void WebImageView::clearCache() {
    //mNetworkDiskCache->clear();
}

QByteArray WebImageView::data(){
    return imageData;
}
