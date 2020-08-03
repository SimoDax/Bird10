#include <QDateTime>
#include <QDebug>

#include "oxtwitter.h"
#include "o0globals.h"
#include "src/QtCurl/CurlEasy.h"
#include <src/QJson4/QJsonArray.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonObject.h>
#include <src/applicationui.hpp>

#include <iostream>

const char XAUTH_USERNAME[] = "x_auth_identifier";
const char XAUTH_PASSWORD[] = "x_auth_password";


OXTwitter::OXTwitter(QObject *parent): O1Twitter(parent) {
}

QString OXTwitter::username() {
    return username_;
}

void OXTwitter::setUsername(const QString &username) {
    username_ = username;
    Q_EMIT usernameChanged();
}

QString OXTwitter::password() {
    return password_;
}

void OXTwitter::setPassword(const QString &password) {
    password_ = password;
    Q_EMIT passwordChanged();
}

void OXTwitter::link() {
    qDebug() << "OXTwitter::link";
    if (linked()) {
        qDebug() << "Linked already";
        return;
    }

    if (username_.isEmpty() || password_.isEmpty()) {
        qWarning() << "Error: XAuth parameters not set. Aborting!";
        emit linkingFailed();
        return;
    }
    // request guest token
    CurlEasy * request = new CurlEasy(NULL, &ApplicationUI::curlThread);
    request->set(CURLOPT_URL, "https://api.twitter.com/1.1/guest/activate.json");
    spoofHeaders(request);
    request->setHttpHeaderRaw(O2_HTTP_AUTHORIZATION_HEADER, "Bearer AAAAAAAAAAAAAAAAAAAAAFXzAwAAAAAAMHCxpeSDG1gLNLghVe8d74hl6k4%3DRUMF4xAQLsbeBhTSRrCiQpJtxoGWeyHrDb5te2jpGskWDFW82F");
    request->set(CURLOPT_POST, true);
    request->set(CURLOPT_POSTFIELDS, "");
    request->set(CURLOPT_VERBOSE, 1L);
    request->set(CURLOPT_FAILONERROR, 0);
    request->setHttpHeaderRaw("Content-Type", QByteArray());
    request->setHttpHeaderRaw("Expect", QByteArray());

    connect(request, SIGNAL(error(CURLcode)), this, SLOT(onTokenRequestError(CURLcode)));
    connect(request, SIGNAL(done(CURLcode)), this, SLOT(onGuestTokenRequestFinished(CURLcode)));

    QMetaObject::invokeMethod(request, "perform", Qt::QueuedConnection);
}

void OXTwitter::onGuestTokenRequestFinished(CURLcode code){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    reply->deleteLater();
    QByteArray data = reply->data();
    QVariantMap response = QJsonDocument::fromJson(reply->data()).object().toVariantMap();
    QString guest_token = response["guest_token"].toString();

    if(guest_token.isNull() || guest_token.isEmpty()){
        emit linkingFailed();
        return;
    }


    // prepare XAuth parameters
    QList<O0RequestParameter> xAuthParams_;
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_identifier"), username_.toUtf8()));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_password"), password_.toUtf8()));
    xAuthParams_.append(O0RequestParameter(QByteArray("send_error_codes"), QByteArray("true")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_login_challenge"), QByteArray("1")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_login_verification"), QByteArray("1")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_country_code"), QByteArray("US")));
    xAuthParams_.append(O0RequestParameter(QByteArray("ui_metrics"), QByteArray()));
    QByteArray fields = createQueryParameters(xAuthParams_);
//    std::cout << QString(fields).toStdString();

//    qDebug()<<"FIELDS: "<<fields;
//
//    QFile file("./shared/documents/bird10log.txt");
//    file.open(QIODevice::Append);
//    if(file.isOpen()){
//        QTextStream out(&file);
//        out << "\n\n --------- \n" ;
//    }
//    file.close();

    // Post request
    CurlEasy * request = new CurlEasy(NULL, &ApplicationUI::curlThread);
    request->set(CURLOPT_URL, "https://api.twitter.com/auth/1/xauth_password.json");
    spoofHeaders(request);
    request->setHttpHeaderRaw(O2_HTTP_AUTHORIZATION_HEADER, "Bearer AAAAAAAAAAAAAAAAAAAAAFXzAwAAAAAAMHCxpeSDG1gLNLghVe8d74hl6k4%3DRUMF4xAQLsbeBhTSRrCiQpJtxoGWeyHrDb5te2jpGskWDFW82F");
    request->setHttpHeaderRaw("Content-Type", O2_MIME_TYPE_XFORM);
    request->setHttpHeaderRaw("Connection", "close");
    request->setHttpHeaderRaw("X-Guest-Token", guest_token.toLatin1());
    request->set(CURLOPT_FAILONERROR, 0);
    request->set(CURLOPT_VERBOSE, 1L);
    request->set(CURLOPT_POST, true);
    request->set(CURLOPT_POSTFIELDSIZE, fields.size());
    request->set(CURLOPT_COPYPOSTFIELDS, fields.constData());
    connect(request, SIGNAL(error(CURLcode)), this, SLOT(onTokenExchangeError(CURLcode)));
    connect(request, SIGNAL(done(CURLcode)), this, SLOT(onTokenRequestFinished(CURLcode)));

    QMetaObject::invokeMethod(request, "perform", Qt::QueuedConnection);
}

void OXTwitter::onTokenRequestFinished(CURLcode code){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    QVariantMap response = (QJsonDocument::fromJson(reply->data()).object().toVariantMap());

//    QFile file("./shared/documents/bird10log.txt");
//    file.open(QIODevice::Append);
//    if(file.isOpen()){
//        QTextStream out(&file);
//        out << "\n" << QDateTime::currentDateTime().toString(Qt::ISODate) << " OXTwitter::onTokenRequestFinished: reply: \n" << QString(reply->data());
//    }
//    file.close();

    if (response.contains(O2_OAUTH_TOKEN) && response.contains(O2_OAUTH_TOKEN_SECRET)) {

        setToken(response.take(O2_OAUTH_TOKEN).value<QString>());
        setTokenSecret(response.take(O2_OAUTH_TOKEN_SECRET).value<QString>());
        setExtraTokens(response);
        user_id = response["user_id"].toString();
        screen_name = response["screen_name"].toString();
        setLinked(true);
        emit linkingSucceeded();
    }
    else if(response.keys().contains("login_verification_request_url")){

//            QFile file("./shared/documents/bird10log.txt");
//            file.open(QIODevice::Append);
//            if(file.isOpen()){
//                QTextStream out(&file);
//                out << "\n" << QDateTime::currentDateTime().toString(Qt::ISODate) << " OXTwitter::onTokenRequestFinished: Opening verification url";
//            }
//            file.close();

            login_verification_user_id = response.take("login_verification_user_id").value<QString>();
            login_verification_request_id = response.take("login_verification_request_id").value<QString>();

            emit openBrowser(response.take("login_verification_request_url").value<QString>());

    }
    else {

        QFile file("./shared/documents/bird10log.txt");
        file.open(QIODevice::Append);
        if(file.isOpen()){
            QTextStream out(&file);
            out << "\n" << QDateTime::currentDateTime().toString(Qt::ISODate) << " OXTwitter::onTokenRequestFinished: oauth_token or oauth_token_secret missing from response";
        }
        file.close();

        qWarning() << "OXTwitter::onTokenRequestFinished: oauth_token or oauth_token_secret missing from response";
        emit linkingFailed();
    }


    reply->deleteLater();
//    }
}

void OXTwitter::onChallengeSuccess(){
    QList<O0RequestParameter> xAuthParams_;
    xAuthParams_.append(O0RequestParameter(QByteArray("login_verification_user_id"), login_verification_user_id.toUtf8()));
    xAuthParams_.append(O0RequestParameter(QByteArray("login_verification_request_id"), login_verification_request_id.toUtf8()));
    xAuthParams_.append(O0RequestParameter(QByteArray("send_error_codes"), QByteArray("true")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_login_challenge"), QByteArray("1")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_login_verification"), QByteArray("1")));
    xAuthParams_.append(O0RequestParameter(QByteArray("x_auth_mode"), QByteArray("client_auth")));
    QByteArray fields = createQueryParameters(xAuthParams_);

    QList<O0RequestParameter> headers;
    headers.append(O0RequestParameter(O2_OAUTH_CONSUMER_KEY, clientId().toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_NONCE, nonce()));
    headers.append(O0RequestParameter(O2_OAUTH_TIMESTAMP, QString::number(QDateTime::currentDateTimeUtc().toTime_t()).toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_VERSION, "1.0"));
    headers.append(O0RequestParameter(O2_OAUTH_SIGNATURE_METHOD, signatureMethod().toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_SIGNATURE, generateSignature(headers, accessTokenUrl(), xAuthParams_, QNetworkAccessManager::PostOperation)));

    CurlEasy * request = new CurlEasy(NULL, &ApplicationUI::curlThread);
    request->set(CURLOPT_URL, accessTokenUrl());
    spoofHeaders(request);
    request->setHttpHeaderRaw(O2_HTTP_AUTHORIZATION_HEADER, buildAuthorizationHeader(headers));
    request->setHttpHeaderRaw("Content-Type", O2_MIME_TYPE_XFORM);
    request->set(CURLOPT_FAILONERROR, 0);
    request->set(CURLOPT_VERBOSE, 1L);
    request->set(CURLOPT_POST, true);
    request->set(CURLOPT_POSTFIELDSIZE, fields.size());
    request->set(CURLOPT_COPYPOSTFIELDS, fields.constData());
    connect(request, SIGNAL(error(CURLcode)), this, SLOT(onTokenExchangeError(CURLcode)));
    connect(request, SIGNAL(done(CURLcode)), this, SLOT(onTokenRequestFinished(CURLcode)));

    QMetaObject::invokeMethod(request, "perform", Qt::QueuedConnection);

    emit closeBrowser();
}


QString OXTwitter::screenName(){
    return extraTokens().value("screen_name").toString();
}
