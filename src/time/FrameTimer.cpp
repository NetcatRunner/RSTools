#include "RST/time/FrameTimer.hpp"

namespace RST::Time {

    FrameTimer::FrameTimer(uint32_t targetFps)
    {
        setTargetFps(targetFps);
        _previousTime = std::chrono::system_clock::now();
    }
    
    FrameTimer::~FrameTimer()
    {
    }

    void FrameTimer::tick() {
        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = currentTime - _previousTime;

        if (_targetFps > 0) {
            double sleepTime = _targetFrameTime - elapsed.count();
            if (sleepTime > 0.0f) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
                currentTime = std::chrono::system_clock::now();
                elapsed = currentTime - _previousTime;
            }
        }

        _deltaTime = elapsed.count();
        _previousTime = currentTime;

    }

    void FrameTimer::setTargetFps(uint32_t newFPS) {
        _targetFps = newFPS;

        _targetFrameTime = (_targetFps > 0) ? 1.0f / static_cast<double>(_targetFps) : 0.0f;
    }
}
