/*
 * Timer.hpp
 *
 *  Created on: 12/dic/2020
 *      Author: Simone
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
