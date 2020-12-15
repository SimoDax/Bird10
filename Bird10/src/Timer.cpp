/*
 * Timer.cpp
 *
 *  Created on: 12/dic/2020
 *      Author: Simone
 */

#include <src/Timer.hpp>
#include <QTimer>
#include <QDateTime>

#include <QDebug>

#define TICK_TIME 16    // The screen doesn't update faster than 60 fps, don't check more times in a single frame..

Timer::Timer(QObject* parent) : QObject(parent)
{
}

void Timer::start()
{
    m_endTime = QDateTime::currentMSecsSinceEpoch() + m_duration;
    m_progress = 0;
    m_state = STARTED;
    QTimer::singleShot(TICK_TIME, this, SLOT(onTick()));
    emit activeChanged();
}

void Timer::stop()
{
    m_state = STOPPED;
    emit activeChanged();
}

void Timer::pause()
{
    m_state = PAUSED;
    m_remainingTime = m_endTime - QDateTime::currentMSecsSinceEpoch();
}

void Timer::resume()
{
    if(m_state == PAUSED){
        m_endTime = QDateTime::currentMSecsSinceEpoch() + m_remainingTime;
        m_state = STARTED;
        QTimer::singleShot(TICK_TIME, this, SLOT(onTick()));
    }
}

void Timer::onTick()
{
    if(m_state == STARTED){
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        m_progress = 1 - (m_endTime - currentTime)/(float)m_duration;
        emit progressChanged();

        if(currentTime >= m_endTime){
            stop();
            emit triggered();
        }
        else
            QTimer::singleShot(TICK_TIME, this, SLOT(onTick()));
    }
}
