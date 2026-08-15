#pragma once

#include "RST/log/LogLevel.hpp"
#include "RST/log/LogMessage.hpp"
#include "RST/log/Logger.hpp"
#include "RST/log/LogRegistry.hpp"
#include "RST/log/ScopedTimer.hpp"

#include "RST/log/Format/Formatter.hpp"

#include "RST/log/Sinks/ISink.hpp"
#include "RST/log/Sinks/NullSink.hpp"
#include "RST/log/Sinks/OStreamSink.hpp"
#include "RST/log/Sinks/CallbackSink.hpp"
#include "RST/log/Sinks/ConsoleSink.hpp"
#include "RST/log/Sinks/FileSink.hpp"
#include "RST/log/Sinks/RotatingFileSink.hpp"

namespace RST::Log {

    namespace detail {
        [[nodiscard]] inline Logger& deref(Logger& logger) noexcept { return logger; }
        [[nodiscard]] inline Logger& deref(Logger* logger) noexcept { return *logger; }

        template <typename Ptr> [[nodiscard]] Logger& deref(const Ptr& pointer) { return *pointer; }
    }
}

#define RST_LOG_L(logger, lvl, ...)                                            \
    do {                                                                       \
        ::RST::Log::Logger& target = ::RST::Log::detail::deref(logger);        \
        if (target.shouldLog(lvl)) {                                           \
            target.log((lvl), RST_SOURCE_LOCATION, __VA_ARGS__);               \
        }                                                                      \
    } while (false)

#define LOG_TRACE_L(logger, ...)  RST_LOG_L(logger, RST::Log::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG_L(logger, ...)  RST_LOG_L(logger, RST::Log::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO_L(logger, ...)   RST_LOG_L(logger, RST::Log::LogLevel::Info,  __VA_ARGS__)
#define LOG_WARN_L(logger, ...)   RST_LOG_L(logger, RST::Log::LogLevel::Warn,  __VA_ARGS__)
#define LOG_ERROR_L(logger, ...)  RST_LOG_L(logger, RST::Log::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL_L(logger, ...)  RST_LOG_L(logger, RST::Log::LogLevel::Fatal, __VA_ARGS__)

#define RST_LOG(lvl, ...) RST_LOG_L(::RST::Log::Registry::defaultLogger(), (lvl), __VA_ARGS__)

#define LOG_TRACE(...)  RST_LOG(RST::Log::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...)  RST_LOG(RST::Log::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...)   RST_LOG(RST::Log::LogLevel::Info,  __VA_ARGS__)
#define LOG_WARN(...)   RST_LOG(RST::Log::LogLevel::Warn,  __VA_ARGS__)
#define LOG_ERROR(...)  RST_LOG(RST::Log::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...)  RST_LOG(RST::Log::LogLevel::Fatal, __VA_ARGS__)
