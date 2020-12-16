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
