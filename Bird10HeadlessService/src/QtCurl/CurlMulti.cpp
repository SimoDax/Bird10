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

// This disables min & max macros declaration in windows.h which will be included somewhere there
#define NOMINMAX

#include "src/QtCurl/CurlMulti.h"
#include <limits>
#include <memory>
#include <QThreadStorage>
#include <QTimer>
#include <QSocketNotifier>
#include <QSharedPointer>
#include "src/QtCurl/CurlEasy.h"
#include <QDebug>
#include <QtGlobal>
#include <QDateTime>
#include <QMutex>

#include <unistd.h>


struct CurlMultiSocket
{
    curl_socket_t socketDescriptor;
    QSocketNotifier *readNotifier;
    QSocketNotifier *writeNotifier;
    QSocketNotifier *errorNotifier;
};

CurlMulti::CurlMulti(QObject *parent)
    : QObject(parent)
    , handle_(0)
    , running_(0)
    , lastTimerHandled(true)
    , newerTimerExists(false)
    , counter(0)
    , mutex(new QMutex(QMutex::NonRecursive))
{

    handle_ = curl_multi_init();
    Q_ASSERT(handle_ != 0);

    //connect(timer_, SIGNAL(timeout()), this, SLOT(curlMultiTimeout()), Qt::DirectConnection);
    //mutex = QMutex(QMutex::Recursive);
    //connect(timer_, SIGNAL(timeout()), timer_, SLOT(deleteLater()));
}

CurlMulti::~CurlMulti()
{
    while (!transfers_.empty()) {
        (*transfers_.begin())->abort();
    }

    if (handle_) {
        curl_multi_cleanup(handle_);
    }
}

void CurlMulti::curlMultiPerform(){
    CURLMsg* msg;
    int msgs_left;

    curl_multi_perform(handle_, &running_);

    while((msg = curl_multi_info_read(handle_, &msgs_left))) {
        if(msg->msg == CURLMSG_DONE) {
            CURL *e = msg->easy_handle;
            curl_multi_remove_handle(handle_, e);

            CurlEasy *easy = 0;
            curl_easy_getinfo(e, CURLINFO_PRIVATE, &easy);

            if (!easy)
                continue;

            easy->onCurlMessage(msg->data.result);
        }
    }

    // If there are still pending requests keep processing them in the next event loop iteration
    if(running_)
        QTimer::singleShot(0, this, SLOT(curlMultiPerform()));
}

CurlMulti *CurlMulti::threadInstance()
{

    static QThreadStorage<QSharedPointer<CurlMulti> > instances;
    if (!instances.hasLocalData()) {
        instances.setLocalData(QSharedPointer<CurlMulti>(new CurlMulti));
    }
    return instances.localData().data();
}

void CurlMulti::addTransfer(CurlEasy *transfer)
{
//    qDebug()<<"Service::CurlMulti::addTransfer";

    transfers_ << transfer;
    curl_multi_add_handle(handle_, transfer->handle());

    // Start the perform loop if it's not running
    if(!running_)
        QTimer::singleShot(0, this, SLOT(curlMultiPerform()));
}

void CurlMulti::removeTransfer(CurlEasy *transfer)
{
    if (transfers_.contains(transfer)) {
        curl_multi_remove_handle(handle_, transfer->handle());
        transfers_.remove(transfer);
    }
}


int CurlMulti::curlTimerFunction(int timeoutMsec)
{
//    if(mutex.tryLock())
//        mutex.unlock();
//    else
//        return 0;
    //mutex->lock();
//
//
//    qDebug()<<QDateTime::currentMSecsSinceEpoch()<<"set timeout for "<<timeoutMsec<<"old timer is"<<timer_->isActive();
//
//    if (timeoutMsec >= 0){
//        //disconnect(timer_, SIGNAL(timeout()), this, SLOT(curlMultiTimeout()));
//        //delete timer_;
//
//        //if(!lastTimerHandled && timeoutMsec < 0)
//        if(!lastTimerHandled && !timer_->isActive())
//            //newerTimerExists = true;
//
//        lastTimerHandled = false;
//        //qDebug()<<"Counter is "<<counter;
//
////            timer_ = new QTimer(this);
////            timer_->setSingleShot(true);
////            connect(timer_, SIGNAL(timeout()), this, SLOT(curlMultiTimeout()));
//
//        timer_->start(qMax(timeoutMsec, 50));
//    }
//    else if (timeoutMsec<0)
//        timer_->stop();

    //mutex->unlock();
    return 0;
}

void CurlMulti::curlMultiTimeout()
{
    mutex->lock();

    if(!newerTimerExists){
        qDebug()<<QDateTime::currentMSecsSinceEpoch()<<"timeout!";
        curlSocketAction(CURL_SOCKET_TIMEOUT, 0);
        lastTimerHandled = true;
        newerTimerExists = false;
        //counter--;
        qDebug()<<QDateTime::currentMSecsSinceEpoch()<<"timeout routine ended!";
    }
    else {
        //counter--;
        newerTimerExists = false;
        qDebug()<<QDateTime::currentMSecsSinceEpoch()<<"timeout prevented!";
    }
    mutex->unlock();
}

void CurlMulti::socketReadyRead(int socketDescriptor)
    { curlSocketAction(socketDescriptor, CURL_CSELECT_IN); }

void CurlMulti::socketReadyWrite(int socketDescriptor)
    { curlSocketAction(socketDescriptor, CURL_CSELECT_OUT); }

void CurlMulti::socketException(int socketDescriptor)
    { curlSocketAction(socketDescriptor, CURL_CSELECT_ERR); }

void CurlMulti::curlSocketAction(curl_socket_t socketDescriptor, int eventsBitmask)
{
    int runningHandles;
    CURLMcode rc = curl_multi_socket_action(handle_, socketDescriptor, eventsBitmask, &runningHandles);
    if (rc != 0) {
        // TODO: Handle global curl errors
        qDebug()<<"rc";
    }


    //qDebug()<<socketDescriptor << " " <<eventsBitmask <<runningHandles;
    if(eventsBitmask == 2)
        qDebug()<<"";
    //usleep(50000);

    int messagesLeft = 0;
    do {
        CURLMsg *message = curl_multi_info_read(handle_, &messagesLeft);

        if (message == 0)
            break;

        if (message->easy_handle == 0)
            continue;

        CurlEasy *transfer = 0;
        curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, &transfer);

        if (transfer == 0)
            continue;

        transfer->onCurlMessage(message);
    } while (messagesLeft);
}



int CurlMulti::staticCurlTimerFunction(CURLM *multiHandle, long timeoutMs, void *userp)
{
    Q_UNUSED(multiHandle);
    CurlMulti *multi = static_cast<CurlMulti*>(userp);
    Q_ASSERT(multi != 0);

    int intTimeoutMs;

    if (timeoutMs >= std::numeric_limits<int>::max())
        intTimeoutMs = std::numeric_limits<int>::max();
    else if(timeoutMs >= 100000)
        intTimeoutMs = 100000;
    else if (timeoutMs >= 0)
        intTimeoutMs = static_cast<int>(timeoutMs);
    else
        intTimeoutMs = -1;

    return multi->curlTimerFunction(intTimeoutMs);
}

