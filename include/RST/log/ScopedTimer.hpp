#pragma once

#include "Logger.hpp"
#include "LogRegistry.hpp"

#include <chrono>
#include <string>
#include <memory>

namespace RST::Log {

    class ScopedTimer {
    public:
        using Clock    = std::chrono::steady_clock;
        using Duration = std::chrono::milliseconds;
    private:
        std::string _name;
        std::shared_ptr<Logger> _logger;

        Duration _warnThreshold;
        LogLevel _level;
        SourceLocation _src;
        Clock::time_point _start;
    public:

        ScopedTimer(const std::string& name, std::shared_ptr<Logger> logger = nullptr, Duration warn_threshold = Duration(-1), LogLevel level = LogLevel::Debug, SourceLocation src = {})
            : _name(std::move(name)), _logger(logger ? std::move(logger) : Registry::defaultLogger()), _warnThreshold(warn_threshold), _level(level), _src(src), _start(Clock::now())
        {}

        ~ScopedTimer() {
            const auto elapsed = std::chrono::duration_cast<Duration>(Clock::now() - _start);

            const bool isWarn = (_warnThreshold.count() >= 0) && (elapsed >= _warnThreshold);

            const LogLevel lvl = isWarn ? LogLevel::Warn : _level;

            _logger->log(lvl, _src, "{}: {}ms{}", _name, elapsed.count(), isWarn ? " [SLOW]" : "");
        }

        ScopedTimer(const ScopedTimer&)            = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;

        Duration elapsed() const {
            return std::chrono::duration_cast<Duration>(Clock::now() - _start);
        };
    };

} // namespace RST::Log

#define LOG_SCOPE_TIME(logger, name) \
    RST::Log::ScopedTimer _rst_timer_##__LINE__(name, logger, \
    RST::Log::ScopedTimer::Duration::zero(), \
    RST::Log::LogLevel::Debug, RST_SOURCE_LOCATION)

#define LOG_SCOPE_TIME_DEFAULT(name) LOG_SCOPE_TIME(nullptr, name)

#define LOG_SCOPE_TIME_WARN(logger, name, threshold_ms) \
    RST::Log::ScopedTimer _rst_timer_##__LINE__(name, logger, \
    std::chrono::milliseconds(threshold_ms), \
    RST::Log::LogLevel::Debug, RST_SOURCE_LOCATION)

