#include "timer.h"

CTimeCounter::CTimeCounter() : timerElapsed{nullptr}, PauseStart(0), PauseTime(0)
{
    timerElapsed = new QElapsedTimer;
}
CTimeCounter::~CTimeCounter()
{
    delete timerElapsed;
}
void CTimeCounter::Start()
{
    PauseStart = 0;
    PauseTime = 0;
    timerElapsed->restart();
}
void CTimeCounter::Pause(bool enable /*= true*/)
{
    if(PauseStart && enable)
        return;
    if(0 == PauseStart && false == enable)
        return;

    if(enable)
    {
        PauseStart = timerElapsed->elapsed();
        return;
    }

    PauseTime += timerElapsed->elapsed() - PauseStart;
    PauseStart = 0;
}
int CTimeCounter::time()
{
    const quint64 diff = PauseStart ? (timerElapsed->elapsed() - PauseStart) : 0;
    return static_cast<int>(timerElapsed->elapsed() - PauseTime - diff);
}
