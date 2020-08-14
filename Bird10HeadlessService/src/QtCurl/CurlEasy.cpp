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

#include "src/QtCurl/CurlEasy.h"
#include "src/QtCurl/CurlMulti.h"
#include <QDebug>
#include <QThread>
#include <cstdio>

//QByteArray CurlEasy::buffer_;
int trace(CURL *handle, curl_infotype type, unsigned char *data, size_t size, void *userp)
{
    fprintf(stderr, "%d: %.*s\n", type, size, data);
    //std::cerr<< data << std::endl;
    return 1;
}


CurlEasy::CurlEasy(QObject *parent, QThread* curlThread)
    : QObject(parent)
    , handle_ (0)
    , lastResult_(CURLE_OK)
    , curlHttpHeaders_(0)
    , httpHeadersWereSet_(false)

{
    handle_ = curl_easy_init();
    Q_ASSERT(handle_ != 0);

    set(CURLOPT_PRIVATE, this);
    set(CURLOPT_WRITEFUNCTION, staticCurlWriteFunction);
    set(CURLOPT_WRITEDATA, &buffer_);

    //set(CURLOPT_VERBOSE, 1L);
    //set(CURLOPT_DEBUGFUNCTION, trace);
    this->moveToThread(curlThread);
}

CurlEasy::~CurlEasy()
{
    removeFromMulti();

    if (handle_) {
        curl_easy_cleanup(handle_);
    }

    if (curlHttpHeaders_) {
        curl_slist_free_all(curlHttpHeaders_);
        curlHttpHeaders_ = nullptr;
    }
}

void CurlEasy::deleteLater()
{
    buffer_.clear();
    removeFromMulti();
    QObject::deleteLater();
}

void CurlEasy::perform()
{
    //TODO check if running

    rebuildCurlHttpHeaders();
    buffer_.clear();

//    CURLcode code = curl_easy_perform(handle_);
//    onCurlMessage(code);

    CurlMulti::threadInstance()->addTransfer(this);
}


void CurlEasy::abort()
{
//    if (!isRunning())
//        return;

    removeFromMulti();

    emit aborted();
}

void CurlEasy::removeFromMulti()
{
    CurlMulti::threadInstance()->removeTransfer(this);
}

void CurlEasy::onCurlMessage(CURLcode code)
{
//    qDebug()<<"Service::CurlEasy::onCurlMessage";
    //qDebug()<<data();
    lastResult_ = code;
    if(lastResult_ == CURLE_OK && get<long>(CURLINFO_RESPONSE_CODE) < 400)
        emit done(lastResult_);
    else
        emit error(lastResult_);
}

void CurlEasy::onCurlMessage(CURLMsg *msg)
{

}

void CurlEasy::rebuildCurlHttpHeaders()
{
    if (!httpHeadersWereSet_)
        return;

    if (curlHttpHeaders_) {
        curl_slist_free_all(curlHttpHeaders_);
        curlHttpHeaders_ = nullptr;
    }

    for (QMap<QString, QByteArray>::const_iterator it = httpHeaders_.begin(); it != httpHeaders_.end(); ++it) {
        const QString &header = it.key();
        const QByteArray &value = it.value();

        QByteArray headerString = header.toUtf8();
        headerString += ": ";
        headerString += value;
        headerString.append(char(0));

        curlHttpHeaders_ = curl_slist_append(curlHttpHeaders_, headerString.constData());
    }

    set(CURLOPT_HTTPHEADER, curlHttpHeaders_);
}


void CurlEasy::setWriteFunction(size_t (CurlEasy::*function)(char*, size_t))
{
    writeFunction_ = function;
    if (writeFunction_) {
        set(CURLOPT_WRITEFUNCTION, staticCurlWriteFunction);
        set(CURLOPT_WRITEDATA, &buffer_);
    } else {
        set(CURLOPT_WRITEFUNCTION, 0);
        set(CURLOPT_WRITEDATA, 0);
    }
}


size_t CurlEasy::staticCurlWriteFunction(char *data, size_t size, size_t nitems, void *easyPtr)
{
    QByteArray *buffer = static_cast<QByteArray*>(easyPtr);
    Q_ASSERT(buffer != 0);

    buffer->append(data, size*nitems);

    //if (easy->writeFunction_)
    //    return (easy->*writeFunction_)(data, size*nitems);
    //else
        return  size*nitems;
}

size_t CurlEasy::staticCurlHeaderFunction(char *data, size_t size, size_t nitems, void *easyPtr)
{
    CurlEasy *easy = static_cast<CurlEasy*>(easyPtr);
    Q_ASSERT(easy != 0);

//    if (easy->headerFunction_)
//        return easy->headerFunction_(data, size*nitems);
//    else
        return  size*nitems;
}


size_t CurlEasy::staticCurlReadFunction(char *buffer, size_t size, size_t nitems, void *easyPtr)
{
    CurlEasy *transfer = static_cast<CurlEasy*>(easyPtr);
    Q_ASSERT(transfer != 0);

//    if (transfer->readFunction_)
//        return transfer->readFunction_(buffer, size*nitems);
//    else
        return size*nitems;
}


void CurlEasy::removeHttpHeader(const QString &header)
{
    httpHeaders_.remove(header);
    httpHeadersWereSet_ = true;
}

QByteArray CurlEasy::httpHeaderRaw(const QString &header) const
{
    return httpHeaders_[header];
}

void CurlEasy::setHttpHeaderRaw(const QString &header, const QByteArray &encodedValue)
{
    httpHeaders_[header] = encodedValue;
    httpHeadersWereSet_ = true;
}

bool CurlEasy::set(CURLoption option, const QString &parameter)
    { return set(option, parameter.toUtf8().constData()); }

bool CurlEasy::set(CURLoption option, const QUrl &parameter)
    { return set(option, parameter.toEncoded().constData()); }

void CurlEasy::setHttpHeader(const QString &header, const QString &value)
    { setHttpHeaderRaw(header, QUrl::toPercentEncoding(value)); }

QString CurlEasy::httpHeader(const QString &header) const
    { return QUrl::fromPercentEncoding(httpHeaders_[header]); }

bool CurlEasy::hasHttpHeader(const QString &header) const
    { return httpHeaders_.contains(header); }
