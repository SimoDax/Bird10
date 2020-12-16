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

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <QObject>

class Timer: public QObject
{
    Q_OBJECT

    typedef enum{
        STOPPED,
        STARTED,
        PAUSED,
    } TimerState;

public:

    Q_PROPERTY(int duration READ duration WRITE setDuration)
    int duration(){ return m_duration;};
    void setDuration(int duration){ m_duration = duration;};

    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    float progress(){ return m_progress;};

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    bool active(){ return m_state != STOPPED;};

    Timer(QObject* parent = 0);

signals:
    void triggered();
    void progressChanged();
    void activeChanged();

public slots:
    void start();
    void stop();
    void pause();
    void resume();

protected slots:
    void onTick();

protected:
    int m_duration, m_remainingTime;
    float m_progress;
    qint64 m_endTime;
    TimerState m_state;

};

#endif /* TIMER_HPP_ */
