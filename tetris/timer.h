#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QElapsedTimer>
#include "options.h"

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

#endif // TIMER_H
