#pragma once

#include "RST/log/LogLevel.hpp"
#include "RST/log/Logger.hpp"
#include "RST/log/SourceLocation.hpp"

/// @namespace RST::Log
/// Thread-safe logging: loggers, sinks, patterns and macros.
///
/// Registry::init() sets up the default logger used by the `RST_LOG_*` macros, and Registry::get()
/// returns named loggers sharing the same sinks. A Logger can also be created and configured by hand.

namespace RST::Log::detail {

    [[nodiscard]] inline Logger& deref(Logger& logger) noexcept { return logger; }
    [[nodiscard]] inline Logger& deref(Logger* logger) noexcept { return *logger; }

    template <typename Pointer>
    [[nodiscard]] Logger& deref(const Pointer& pointer) noexcept { return *pointer; }

}

/// Lowest level compiled in, from 0 (`Trace`) to 5 (`Fatal`): logging macros below it compile to nothing.
#ifndef RST_LOG_ACTIVE_LEVEL
#  define RST_LOG_ACTIVE_LEVEL 0
#endif

/// Logs a `std::format` message on `logger`, a reference or a pointer, with the source location.
///
/// The arguments are only evaluated when `level` is enabled.
#define RST_LOG_L(logger, level, ...)                                                           \
    do {                                                                                        \
        ::RST::Log::Logger& rstLogTarget_ = ::RST::Log::detail::deref(logger);                  \
        if (rstLogTarget_.shouldLog(level)) {                                                   \
            rstLogTarget_.log((level), RST_SOURCE_LOCATION, __VA_ARGS__);                       \
        }                                                                                       \
    } while (false)

#define RST_LOG_DISCARDED_L(logger, ...)                                                        \
    do {                                                                                        \
        if constexpr (false) {                                                                  \
            RST_LOG_L(logger, ::RST::Log::LogLevel::Trace, __VA_ARGS__);                        \
        }                                                                                       \
    } while (false)

/// @name Macros on a given logger
/// Log at a fixed level on `logger`, like RST_LOG_L().
/// @{

#if RST_LOG_ACTIVE_LEVEL <= 0
#  define RST_LOGGER_TRACE(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Trace, __VA_ARGS__)
#else
#  define RST_LOGGER_TRACE(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
#if RST_LOG_ACTIVE_LEVEL <= 1
#  define RST_LOGGER_DEBUG(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Debug, __VA_ARGS__)
#else
#  define RST_LOGGER_DEBUG(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
#if RST_LOG_ACTIVE_LEVEL <= 2
#  define RST_LOGGER_INFO(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Info, __VA_ARGS__)
#else
#  define RST_LOGGER_INFO(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
#if RST_LOG_ACTIVE_LEVEL <= 3
#  define RST_LOGGER_WARN(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Warn, __VA_ARGS__)
#else
#  define RST_LOGGER_WARN(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
#if RST_LOG_ACTIVE_LEVEL <= 4
#  define RST_LOGGER_ERROR(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Error, __VA_ARGS__)
#else
#  define RST_LOGGER_ERROR(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
#if RST_LOG_ACTIVE_LEVEL <= 5
#  define RST_LOGGER_FATAL(logger, ...) RST_LOG_L(logger, ::RST::Log::LogLevel::Fatal, __VA_ARGS__)
#else
#  define RST_LOGGER_FATAL(logger, ...) RST_LOG_DISCARDED_L(logger, __VA_ARGS__)
#endif
/// @}

// The unprefixed names of RSTools 1.x, for code not yet migrated. Off by default: LOG_INFO
// and LOG_DEBUG collide with <syslog.h>.
#if defined(RST_LOG_LEGACY_MACROS)
#  define LOG_TRACE_L(logger, ...) RST_LOGGER_TRACE(logger, __VA_ARGS__)
#  define LOG_DEBUG_L(logger, ...) RST_LOGGER_DEBUG(logger, __VA_ARGS__)
#  define LOG_INFO_L(logger, ...)  RST_LOGGER_INFO(logger, __VA_ARGS__)
#  define LOG_WARN_L(logger, ...)  RST_LOGGER_WARN(logger, __VA_ARGS__)
#  define LOG_ERROR_L(logger, ...) RST_LOGGER_ERROR(logger, __VA_ARGS__)
#  define LOG_FATAL_L(logger, ...) RST_LOGGER_FATAL(logger, __VA_ARGS__)
#endif
