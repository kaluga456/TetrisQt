#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QElapsedTimer>
#include "options.h"
//////////////////////////////////////////////////////////////////////////////
class QKeyTimer : public QTimer
{
public:
    QKeyTimer(QObject *parent = nullptr) : QTimer(parent) {}

    bool Start()
    {
        if(isActive())
            return false;
        start(Options.KeySpeed);
        return true;
    }
};
//////////////////////////////////////////////////////////////////////////////
//tick timer values
constexpr int MIN_TICK_INTERVAL = 100; //ms
constexpr int MAX_TICK_INTERVAL = 1000; //ms
constexpr int BOOST_INTERVAL = 30000; //ms
constexpr int BOOST_TICK = (MAX_TICK_INTERVAL - MIN_TICK_INTERVAL) / 10; //increase speed 10 times for each BOOST_INTERVAL
//////////////////////////////////////////////////////////////////////////////
class QTickTimer : public QTimer
{
public:
    QTickTimer(QObject *parent = nullptr) : QTimer(parent) {}

    //speed in %
    int GetSpeed() const;

    void Start();
    bool BoostUp(int round_time);
    void OnTick();

private:
    bool Boost{false}; //true - increase speed on next tick
    int Interval{MAX_TICK_INTERVAL};
    int BoostTime{0};
};
//////////////////////////////////////////////////////////////////////////////
class CTimeCounter
{
public:
    CTimeCounter();
    ~CTimeCounter();

    void Start();
    void Pause(bool enable = true);

    int time(); //milliseconds

private:
    qint64 PauseStart{0}; //milliseconds, nonzero if in paused state
    qint64 PauseTime{0}; //milliseconds, total time in paused state
    QElapsedTimer* timerElapsed{nullptr};
};
//////////////////////////////////////////////////////////////////////////////
#endif // TIMER_H
