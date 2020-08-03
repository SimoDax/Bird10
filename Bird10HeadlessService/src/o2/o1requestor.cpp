#include <QDebug>
#include <QDateTime>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "o1requestor.h"
#include "o1timedreply.h"
#include "o0globals.h"
#include <src/service.hpp>
#include <QStringList>

O1Requestor::O1Requestor(O1 *authenticator, QObject *parent): QObject(parent) {
    authenticator_ = authenticator;
}

CurlEasy* O1Requestor::get(const QString& url, const QList<O0RequestParameter> &signingParameters) {
    CurlEasy* request = setup(url, signingParameters, QNetworkAccessManager::GetOperation);

    QString url_ = url.split("?")[0]; // compatibility with old interface

    if(!signingParameters.isEmpty()){   // create the url query string from the signing parameters
        url_ += "?" + authenticator_->createQueryParameters(signingParameters);
        request->set(CURLOPT_URL, url_);
    }
    //request->set(CURLOPT_VERBOSE, 1L);
    //request->set(CURLOPT_FAILONERROR, 1L);    // Http error now checked on CurlEasy to get reply body
    //request->perform();
    return request;
    //return addTimer(manager_->get(request));
}

CurlEasy* O1Requestor::post(const QString& url, const QList<O0RequestParameter> &signingParameters, const QByteArray &data) {
    CurlEasy* request = setup(url, signingParameters, QNetworkAccessManager::PostOperation);
    QByteArray fields = authenticator_->createQueryParameters(signingParameters);

    //qDebug()<<fields<<fields.size();

    //request->set(CURLOPT_VERBOSE, 1L);
    //request->set(CURLOPT_FAILONERROR, 1L);    // Http error now checked on CurlEasy to get reply body
    request->set(CURLOPT_POST, true);
    request->set(CURLOPT_POSTFIELDSIZE, fields.size());
    request->set(CURLOPT_COPYPOSTFIELDS, fields.constData());
    //request->perform();
    return request;
    //return addTimer(manager_->post(request, data));
}

CurlEasy* O1Requestor::post(const QNetworkRequest &req, const QList<O0RequestParameter> &signingParameters, QHttpMultiPart * multiPart) {
    //TODO: implement multipart
    //CurlEasy* request = setup(req, signingParameters, QNetworkAccessManager::PostOperation);
    //return addTimer(manager_->post(request, multiPart));
    return 0;
}

CurlEasy* O1Requestor::put(const QNetworkRequest &req, const QList<O0RequestParameter> &signingParameters, const QByteArray &data) {
    //TODO: implement put
    //CurlEasy* request = setup(req, signingParameters, QNetworkAccessManager::PutOperation);
    //return addTimer(manager_->put(request, data));
    return 0;
}

CurlEasy* O1Requestor::addTimer(CurlEasy* reply) {
    //(void)new O1TimedReply(reply);
    return reply;
}

CurlEasy* O1Requestor::setup(const QString& url, const QList<O0RequestParameter> &signingParameters, QNetworkAccessManager::Operation operation) {
    // Collect OAuth parameters
    QList<O0RequestParameter> oauthParams;
    oauthParams.append(O0RequestParameter(O2_OAUTH_CONSUMER_KEY, authenticator_->clientId().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_VERSION, "1.0"));
    oauthParams.append(O0RequestParameter(O2_OAUTH_TOKEN, authenticator_->token().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_SIGNATURE_METHOD, authenticator_->signatureMethod().toLatin1()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_NONCE, O1::nonce()));
    oauthParams.append(O0RequestParameter(O2_OAUTH_TIMESTAMP, QString::number(QDateTime::currentDateTimeUtc().toTime_t()).toLatin1()));

    // Add signature parameter
    oauthParams.append(O0RequestParameter(O2_OAUTH_SIGNATURE, authenticator_->generateSignature(oauthParams, url, signingParameters, operation)));

    // Return a copy of the original request with authorization header set
    CurlEasy * request = new CurlEasy(NULL, &Service::curlThread);
    request->set(CURLOPT_URL, url);
    request->setHttpHeaderRaw(O2_HTTP_AUTHORIZATION_HEADER, O1::buildAuthorizationHeader(oauthParams));
    request->setHttpHeader("X-Twitter-Client-Language", QLocale::system().uiLanguages()[0]);
    O1::spoofHeaders(request);
    request->set(CURLOPT_ACCEPT_ENCODING, "");      //Data saving
    return request;
}


