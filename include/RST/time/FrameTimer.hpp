#pragma once

#include <chrono>
#include <thread>

#include <cstdint>

namespace RST::Time {
    class FrameTimer
    {
    private:
        uint32_t _targetFps;

        std::chrono::time_point<std::chrono::system_clock> _previousTime;
        double _deltaTime = 0;
        double _targetFrameTime;
    public:
        FrameTimer(/* args */) = delete;
        FrameTimer(uint32_t targetFps);
        ~FrameTimer();

        void tick();

        double getDeltaTime() const { return _deltaTime; };

        double getCurrentFps() const { return (_deltaTime > 0.0) ? (1.0 / _deltaTime) : 0.0; };

        void setTargetFps(uint32_t newFPS);
    };
    
}