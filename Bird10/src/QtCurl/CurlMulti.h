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

#ifndef CURLMULTI_H
#define CURLMULTI_H

#include <curl/curl.h>
#include <QObject>
#include <QSet>
#include <QMutex>

class QTimer;
class CurlEasy;
struct CurlMultiSocket;

class CurlMulti : public QObject
{
    Q_OBJECT
public:
    explicit CurlMulti(QObject *parent = 0);
    virtual ~CurlMulti();

    static CurlMulti* threadInstance();

    void addTransfer(CurlEasy *transfer);
    void removeTransfer(CurlEasy *transfer);

protected slots:
    void curlMultiPerform();
    void curlMultiTimeout();
    void socketReadyRead(int socketDescriptor);
    void socketReadyWrite(int socketDescriptor);
    void socketException(int socketDescriptor);

protected:
    void curlSocketAction(curl_socket_t socketDescriptor, int eventsBitmask);
    int curlTimerFunction(int timeoutMsec);
    int curlSocketFunction(CURL *easyHandle, curl_socket_t socketDescriptor, int action, CurlMultiSocket *socket);
    static int staticCurlTimerFunction(CURLM *multiHandle, long timeoutMs, void *userp);
    static int staticCurlSocketFunction(CURL *easyHandle, curl_socket_t socketDescriptor, int what, void *userp, void *sockp);

    CURLM *handle_;
    int running_;

    int newerTimerExists, lastTimerHandled, counter;
    QMutex* mutex;

    QSet<CurlEasy*> transfers_;
};

#endif // CURLMULTIINTERFACE_H
