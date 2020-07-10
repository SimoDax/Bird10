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

#include <src/WebImageView/AuthorizedWebImageView.hpp>
#include <src/o2/o0globals.h>
#include <src/applicationui.hpp>

AuthorizedWebImageView::AuthorizedWebImageView()
{
}

void AuthorizedWebImageView::setUrl(const QUrl& url) {

    if(url.isEmpty())
        return;

    // Either it's already set or is waiting for server reply -> no need to do anything
    if(mUrl == url)
        return;

    // Variables
    //mUrl = url;   //Now set only after the image has been received
    mDesiredUrl = url;

//    qDebug()<<url;

    if(!authenticator_) //If authenticator hasnt been set yet just set the url, otherwise send the request
        return;

    // Reset the image
    resetImage();

    // Create request
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setUrl(url);

//    qDebug()<<"Current thread: "<<QThread::currentThread()<<" Auth thread: "<<authenticator_->thread();

    QList<O0RequestParameter> oauthParams;
    oauthParams.append(O0RequestParameter(O2_OAUTH_CONSUMER_KEY, authenticator_->clientId().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_VERSION, "1.0"));
    oauthParams.append(O0RequestParameter(O2_OAUTH_TOKEN, authenticator_->token().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_SIGNATURE_METHOD, authenticator_->signatureMethod().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_NONCE, O1::nonce()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_TIMESTAMP, QString::number(QDateTime::currentDateTimeUtc().toTime_t()).toLatin1()));

    // Add signature parameter
    oauthParams.append(O0RequestParameter(O2_OAUTH_SIGNATURE, authenticator_->generateSignature(oauthParams, url, QList<O0RequestParameter>(), QNetworkAccessManager::GetOperation)));

    request.setRawHeader("Authorization", O1::buildAuthorizationHeader(oauthParams));

    // Create reply
    QNetworkReply * reply = mNetManager->get(request);

    // Connect to signals
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));

    //bool ok = QObject::connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslError(const QList<QSslError>&)));
    //Q_ASSERT(ok);

    emit urlChanged();
}

const QUrl& AuthorizedWebImageView::url() const {
    return mUrl;
}


void AuthorizedWebImageView::onError(){
//    CurlEasy *reply = qobject_cast<CurlEasy *>(sender());
//
//    qDebug()<<"error: " <<reply->get<long>(CURLINFO_RESPONSE_CODE) << ": " << reply->data();

    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    qDebug()<<"error: " <<reply->error()<< ": " <<reply->errorString();

    reply->deleteLater();
}

AuthorizedWebImageView::~AuthorizedWebImageView()
{
    // TODO Auto-generated destructor stub
}

void AuthorizedWebImageView::setAuthenticator(OXTwitter* v)
{
    authenticator_ = v;
    if(!mDesiredUrl.isEmpty())
        setUrl(mDesiredUrl);
}
