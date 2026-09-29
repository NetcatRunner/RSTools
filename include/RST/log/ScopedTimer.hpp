#pragma once

#include "RST/log/Log.hpp"
#include "RST/log/LogRegistry.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

namespace RST::Log {

    class ScopedTimer {
    public:
        using Clock = std::chrono::steady_clock;
        using Duration = std::chrono::milliseconds;

        ScopedTimer(std::string name, std::shared_ptr<Logger> logger = nullptr, Duration warnThreshold = Duration(-1), LogLevel level = LogLevel::Debug, SourceLocation source = {})
            : _name(std::move(name)),
              _logger(logger ? std::move(logger) : Registry::defaultLogger()),
              _warnThreshold(warnThreshold),
              _level(level),
              _source(source),
              _start(Clock::now())
        {}

        ~ScopedTimer()
        {
            const Duration took = elapsed();
            const bool slow = _warnThreshold.count() >= 0 && took >= _warnThreshold;
            _logger->log(slow ? LogLevel::Warn : _level, _source, "{}: {}ms{}", _name, took.count(), slow ? " [SLOW]" : "");
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;

        [[nodiscard]] Duration elapsed() const
        {
            return std::chrono::duration_cast<Duration>(Clock::now() - _start);
        }

    private:
        std::string _name;
        std::shared_ptr<Logger> _logger;
        Duration _warnThreshold;
        LogLevel _level;
        SourceLocation _source;
        Clock::time_point _start;
    };

}

#define RST_LOG_CONCAT_IMPL(a, b) a##b
#define RST_LOG_CONCAT(a, b) RST_LOG_CONCAT_IMPL(a, b)

#define RST_LOG_SCOPE_TIME(logger, name)                                                        \
    ::RST::Log::ScopedTimer RST_LOG_CONCAT(rstScopedTimer, __LINE__)(name, logger,              \
        ::RST::Log::ScopedTimer::Duration(-1), ::RST::Log::LogLevel::Debug, RST_SOURCE_LOCATION)

#define RST_LOG_SCOPE_TIME_DEFAULT(name) RST_LOG_SCOPE_TIME(nullptr, name)

#define RST_LOG_SCOPE_TIME_WARN(logger, name, thresholdMs)                                      \
    ::RST::Log::ScopedTimer RST_LOG_CONCAT(rstScopedTimer, __LINE__)(name, logger,              \
        std::chrono::milliseconds(thresholdMs), ::RST::Log::LogLevel::Debug, RST_SOURCE_LOCATION)

#if defined(RST_LOG_LEGACY_MACROS)
#  define LOG_SCOPE_TIME(logger, name) RST_LOG_SCOPE_TIME(logger, name)
#  define LOG_SCOPE_TIME_DEFAULT(name) RST_LOG_SCOPE_TIME_DEFAULT(name)
#  define LOG_SCOPE_TIME_WARN(logger, name, thresholdMs) RST_LOG_SCOPE_TIME_WARN(logger, name, thresholdMs)
#endif
