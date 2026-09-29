#include "RST/time/FrameTimer.hpp"

#include <thread>

namespace RST::Time {

    namespace {

        constexpr double kAverageWeight = 0.1;

    }

    FrameTimer::FrameTimer(std::uint32_t targetFps) noexcept
        : _startTime(Clock::now()), _previousTime(_startTime)
    {
        setTargetFps(targetFps);
    }

    void FrameTimer::tick() noexcept
    {
        if (_targetFps > 0) {
            const Clock::time_point deadline = _previousTime + _targetFrameTime;
            std::this_thread::sleep_until(deadline - std::chrono::milliseconds(1));
            while (Clock::now() < deadline)
                std::this_thread::yield();
        }

        const Clock::time_point now = Clock::now();
        _deltaTime = std::chrono::duration<double>(now - _previousTime).count();
        _previousTime = now;

        _averageFrameTime = (_frameCount == 0) ? _deltaTime : _averageFrameTime + kAverageWeight * (_deltaTime - _averageFrameTime);
        ++_frameCount;
    }

    void FrameTimer::setTargetFps(std::uint32_t targetFps) noexcept
    {
        _targetFps = targetFps;
        _targetFrameTime = (targetFps > 0)
            ? std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / targetFps))
            : Clock::duration::zero();
    }
}
