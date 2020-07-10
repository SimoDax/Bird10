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

#ifndef CURLEASY_H
#define CURLEASY_H

#include <functional>
#include <curl/curl.h>
#include <QMap>
#include <QObject>
#include <QUrl>
#include <QThread>
#include "src/QtCurl/CurlMulti.h"

#define nullptr 0;


class CurlEasy : public QObject
{
    Q_OBJECT
public:

    explicit CurlEasy(QObject *parent = 0, QThread* curlThread = 0);
    virtual ~CurlEasy();

    Q_SLOT void perform();
    void abort();
    CURLcode result() { return lastResult_; }
    QByteArray data() { return buffer_;}

    // For the list of available set options and valid parameter types consult curl_easy_setopt manual
    template<typename T> bool set(CURLoption option, T parameter) { return curl_easy_setopt(handle_, option, parameter) == CURLE_OK; }
    bool set(CURLoption option, const QString &parameter); // Convenience override for const char* parameters
    bool set(CURLoption option, const QUrl &parameter); // Convenience override for const char* parameters
    void setWriteFunction(size_t (CurlEasy::*function)(char*, size_t));

    // For the list of available get options and valid parameter types consult curl_easy_getinfo manual
    template<typename T> bool get(CURLINFO info, T *pointer) { return curl_easy_getinfo(handle_, info, pointer) == CURLE_OK; }
    template<typename T> T get(CURLINFO info);


    QString httpHeader(const QString &header) const;
    void setHttpHeader(const QString &header, const QString &value);
    bool hasHttpHeader(const QString &header) const;
    void removeHttpHeader(const QString &header);

    QByteArray httpHeaderRaw(const QString &header) const;
    void setHttpHeaderRaw(const QString &header, const QByteArray &encodedValue);

    CURL* handle() { return handle_; }

    // Safety hack: substitue QObject's deleteLater whenever possible to make sure
    // that no callbacks will be called between deleteLater and curl handle removal.
    Q_SLOT void deleteLater();

signals:
    void aborted();
    void progress(qint64 downloadTotal, qint64 downloadNow, qint64 uploadTotal, qint64 uploadNow);
    void done(CURLcode result);
    void error(CURLcode result);

protected:
    void removeFromMulti();
    void onCurlMessage(CURLcode code);
    void onCurlMessage(CURLMsg *msg);
    void rebuildCurlHttpHeaders();

    static size_t staticCurlReadFunction(char *data, size_t size, size_t nitems, void *easyPtr);
    static size_t staticCurlWriteFunction(char *data, size_t size, size_t nitems, void *easyPtr);
    static size_t staticCurlHeaderFunction(char *data, size_t size, size_t nitems, void *easyPtr);


    CURL            *handle_;
    CURLcode        lastResult_;
    size_t (CurlEasy::*writeFunction_)(char*, size_t);

    bool                        httpHeadersWereSet_;
    QMap<QString, QByteArray>   httpHeaders_;
    struct curl_slist*          curlHttpHeaders_;

    QByteArray buffer_;

    static CurlMulti* multi_;

    friend class CurlMulti;
};

template<typename T> T CurlEasy::get(CURLINFO info)
{
    T parameter;
    get(info, &parameter);
    return parameter;
}

#endif // CURLTRANSFER_H
