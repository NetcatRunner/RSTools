#include "RST/time/Timer.hpp"

namespace RST::Time {

    Timer::Timer(/* args */)
    {
    }
    
    Timer::~Timer()
    {
    }

    void Timer::start()
    {
        _startTime = std::chrono::system_clock::now();
        _isRunning = true;
    }

    void Timer::stop()
    {
        _endTime = std::chrono::system_clock::now();
        _isRunning = false;
    }
}
