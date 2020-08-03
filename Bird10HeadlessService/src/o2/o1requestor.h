#ifndef O1REQUESTOR_H
#define O1REQUESTOR_H

#include <QObject>
#include <QNetworkRequest>
#include <QByteArray>

#include "o0export.h"
#include "o1.h"

class QNetworkAccessManager;
class QNetworkReply;
class O1;

/// Makes authenticated requests using OAuth 1.0.
class O0_EXPORT O1Requestor: public QObject {
    Q_OBJECT

public:
    explicit O1Requestor(O1 *authenticator, QObject *parent = 0);

public Q_SLOTS:
    /// Make a GET request.
    /// @param  req                 Network request.
    /// @param  signingParameters   Extra (non-OAuth) parameters participating in signing.
    /// @return Reply.
    CurlEasy* get(const QString& url, const QList<O0RequestParameter> &signingParameters);

    /// Make a POST request.
    /// @param  req                 Network request.
    /// @param  signingParameters   Extra (non-OAuth) parameters participating in signing.
    /// @param  data                Request payload.
    /// @return Reply.
    CurlEasy* post(const QString& url, const QList<O0RequestParameter> &signingParameters, const QByteArray &data = QByteArray());

    /// Make a POST request.
    /// @param  req                 Network request.
    /// @param  signingParameters   Extra (non-OAuth) parameters participating in signing.
    /// @param  multiPart           HTTPMultiPart.
    /// @return Reply.
    CurlEasy* post(const QNetworkRequest &req, const QList<O0RequestParameter> &signingParameters, QHttpMultiPart *multiPart);

    /// Make a PUT request.
    /// @param  req                 Network request.
    /// @param  signingParameters   Extra (non-OAuth) parameters participating in signing.
    /// @param  data                Request payload.
    /// @return Reply.
    CurlEasy* put(const QNetworkRequest &req, const QList<O0RequestParameter> &signingParameters, const QByteArray &data);

protected:
    /// Return new request based on the original, with the "Authentication:" header added.
    CurlEasy* setup(const QString& url, const QList<O0RequestParameter> &signingParameters, QNetworkAccessManager::Operation operation);

    /// Augment reply with a timer.
    CurlEasy* addTimer(CurlEasy* reply);

    O1 *authenticator_;
};


#endif // O1REQUESTOR_H
