#pragma once

#include <chrono>

namespace RST::Time {
    class Timer
    {
    private:
        std::chrono::time_point<std::chrono::system_clock> _startTime;
        std::chrono::time_point<std::chrono::system_clock> _endTime;
        bool _isRunning = false;

        template <typename T, typename R>
        T getElapsedTime() const {
            std::chrono::time_point<std::chrono::system_clock> endTime = _isRunning ? std::chrono::system_clock::now() : _endTime;

            std::chrono::duration<T, R> elapsed = endTime - _startTime;
            return elapsed.count();
        }
    public:
        Timer(/* args */);
        ~Timer();

        void start();
        void stop();

        template <typename T>
        T getElapsedMs() const { return getElapsedTime<T, std::milli>(); };

        template <typename T>
        T getElapsedSeconds() const{ return getElapsedTime<T, std::ratio<1>>(); };
    };
}