#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

class Timer
{
public:
    Timer();

    void start();
    void stop();
    void pause();
    void unpause();

    Uint32 getTicks();

    bool isStarted();
    bool isPaused();

private:
    Uint32 m_startTicks{0};
    Uint32 m_pausedTicks{0};
    
    bool m_paused{false};
    bool m_started{false};
};

#endif
