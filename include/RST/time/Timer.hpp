#pragma once

#include <chrono>
#include <ratio>

namespace RST::Time {

    class Timer {
    public:
        using Clock = std::chrono::steady_clock;
        using Duration = Clock::duration;

        Timer() noexcept = default;

        [[nodiscard]] static Timer startNew() noexcept
        {
            Timer timer;
            timer.start();
            return timer;
        }

        void start() noexcept
        {
            _accumulated = Duration::zero();
            _startTime = Clock::now();
            _isRunning = true;
        }

        void stop() noexcept
        {
            if (_isRunning) {
                _accumulated += Clock::now() - _startTime;
                _isRunning = false;
            }
        }

        void resume() noexcept
        {
            if (!_isRunning) {
                _startTime = Clock::now();
                _isRunning = true;
            }
        }

        void reset() noexcept
        {
            _accumulated = Duration::zero();
            _isRunning = false;
        }

        Duration restart() noexcept
        {
            const Clock::time_point now = Clock::now();
            const Duration elapsedTime = _isRunning ? _accumulated + (now - _startTime) : _accumulated;
            _accumulated = Duration::zero();
            _startTime = now;
            _isRunning = true;
            return elapsedTime;
        }

        [[nodiscard]] bool isRunning() const noexcept { return _isRunning; }

        [[nodiscard]] Duration elapsed() const noexcept
        {
            return _isRunning ? _accumulated + (Clock::now() - _startTime) : _accumulated;
        }

        template <typename T = double>
        [[nodiscard]] T getElapsedSeconds() const noexcept { return getElapsedTime<T, std::ratio<1>>(); }

        template <typename T = double>
        [[nodiscard]] T getElapsedMs() const noexcept { return getElapsedTime<T, std::milli>(); }

        template <typename T = double>
        [[nodiscard]] T getElapsedUs() const noexcept { return getElapsedTime<T, std::micro>(); }

        template <typename T = double>
        [[nodiscard]] T getElapsedNs() const noexcept { return getElapsedTime<T, std::nano>(); }

    private:
        template <typename T, typename Period>
        [[nodiscard]] T getElapsedTime() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::duration<T, Period>>(elapsed()).count();
        }

        Clock::time_point _startTime{};
        Duration _accumulated{};
        bool _isRunning = false;
    };
}
