#pragma once

#include <chrono>
#include <cstdint>

namespace RST::Time {

    /// Measures the frames of a game or render loop, and can cap the frame rate.
    /// @code
    /// RST::Time::FrameTimer frames(60);
    /// while (running) {
    ///     frames.tick();
    ///     update(frames.getDeltaTime());
    /// }
    /// @endcode
    class FrameTimer {
    public:
        using Clock = std::chrono::steady_clock;

        /// Starts timing; `targetFps` caps the frame rate, 0 means no cap.
        explicit FrameTimer(std::uint32_t targetFps = 0) noexcept;

        /// Ends a frame, after sleeping until the target frame time if there is one.
        void tick() noexcept;

        /// Seconds between the last two ticks.
        [[nodiscard]] double getDeltaTime() const noexcept { return _deltaTime; }

        [[nodiscard]] double getCurrentFps() const noexcept { return (_deltaTime > 0.0) ? (1.0 / _deltaTime) : 0.0; }

        /// Frame rate smoothed over the recent frames.
        [[nodiscard]] double getAverageFps() const noexcept { return (_averageFrameTime > 0.0) ? (1.0 / _averageFrameTime) : 0.0; }

        [[nodiscard]] std::uint64_t getFrameCount() const noexcept { return _frameCount; }

        /// Seconds from construction to the last tick.
        [[nodiscard]] double getTotalTime() const noexcept { return std::chrono::duration<double>(_previousTime - _startTime).count(); }

        void setTargetFps(std::uint32_t targetFps) noexcept;
        [[nodiscard]] std::uint32_t getTargetFps() const noexcept { return _targetFps; }

    private:
        Clock::time_point _startTime;
        Clock::time_point _previousTime;
        Clock::duration _targetFrameTime{};
        double _deltaTime = 0.0;
        double _averageFrameTime = 0.0;
        std::uint64_t _frameCount = 0;
        std::uint32_t _targetFps = 0;
    };

}
