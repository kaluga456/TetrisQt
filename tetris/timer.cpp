#include "timer.h"
//////////////////////////////////////////////////////////////////////////////
int QTickTimer::GetSpeed() const
{
    int result = static_cast<int>(100.f * (MAX_TICK_INTERVAL - Interval) / (MAX_TICK_INTERVAL - MIN_TICK_INTERVAL));
    if(result > 100)
        return 100;
    return result;
}
void QTickTimer::Start()
{
    Interval = MAX_TICK_INTERVAL;
    Boost = false;
    BoostTime = 0;
    start(Interval);
}
bool QTickTimer::BoostUp(int round_time)
{
    if(Boost || Interval <= MIN_TICK_INTERVAL)
        return false;

    if(round_time - BoostTime >= BOOST_INTERVAL)
    {
        Interval -= BOOST_TICK;
        BoostTime = round_time;
        Boost = true;
    }
    return true;
}
void QTickTimer::OnTick()
{
    if(false == Boost)
        return;
    start(Interval);
    Boost = false;
}
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////
