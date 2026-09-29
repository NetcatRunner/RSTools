#pragma once

#include "RST/log/LogLevel.hpp"
#include "RST/log/SourceLocation.hpp"

#include <atomic>
#include <cstddef>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace RST::Log {

    class ISink;
    class RingBufferSink;
    struct LogMessage;

    /// Named logger that formats messages and sends them to its sinks; thread-safe.
    ///
    /// Messages use `std::format` syntax, checked at compile time, and are skipped before any
    /// formatting when their level is below getLevel().
    class Logger {
    public:
        /// Creates a logger without sinks; add them with addSink().
        explicit Logger(std::string name, LogLevel level = LogLevel::Trace);
        ~Logger();

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        [[nodiscard]] const std::string& getName() const noexcept { return _name; }

        // ── Levels ───────────────────────────────────────────────────────────
        /// Minimum level to log; `LogLevel::Off` disables the logger.
        void setLevel(LogLevel level) noexcept { _level.store(level, std::memory_order_relaxed); }
        [[nodiscard]] LogLevel getLevel() const noexcept { return _level.load(std::memory_order_relaxed); }
        /// Whether a message at `level` would be logged.
        [[nodiscard]] bool shouldLog(LogLevel level) const noexcept { return level != LogLevel::Off && level >= getLevel(); }

        // ── Flush ───────────────────────────────────────────────────────────
        /// Flushes the sinks after every message at `level` or above.
        void flushOn(LogLevel level) noexcept { _flushLevel.store(level, std::memory_order_relaxed); }
        [[nodiscard]] LogLevel getFlushLevel() const noexcept { return _flushLevel.load(std::memory_order_relaxed); }

        // ── Sinks ────────────────────────────────────────────────────────────
        void addSink(std::shared_ptr<ISink> sink);
        /// Adds `sink` after replaying the messages kept by `history` into it.
        void addSink(std::shared_ptr<ISink> sink, RingBufferSink& history);
        /// Removes `sink`; returns false if it was not attached.
        bool removeSink(const std::shared_ptr<ISink>& sink) noexcept;
        void clearSinks() noexcept;
        [[nodiscard]] std::size_t sinkCount() const noexcept;

        // ── Formatted logging ────────────────────────────────────────────────
        /// Logs a message at `level` with its source location.
        template <class... Args>
        void log(LogLevel level, const SourceLocation& source, std::format_string<Args...> format, Args&&... args) noexcept
        {
            if (shouldLog(level)) {
                vlog(level, source, _name, format.get(), std::make_format_args(args...));
            }
        }

        /// Same as log() with a runtime format string, for wrappers.
        void vlog(LogLevel level, const SourceLocation& source, std::string_view category, std::string_view format, std::format_args args) noexcept;

        /// Logs at a fixed level: trace(), debug(), info(), warn(), error() and fatal().
        template <class... Args> void trace(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Trace, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void debug(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Debug, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void info(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Info, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void warn(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Warn, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void error(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Error, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void fatal(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Fatal, {}, format, std::forward<Args>(args)...); }

        // ── Pre-formatted text ───────────────────────────────────────────────
        /// Logs `message` as is, without formatting.
        void log(LogLevel level, std::string_view message, const SourceLocation& source = {}) noexcept;

        void trace(std::string_view message) noexcept { log(LogLevel::Trace, message); }
        void debug(std::string_view message) noexcept { log(LogLevel::Debug, message); }
        void info(std::string_view message) noexcept { log(LogLevel::Info, message); }
        void warn(std::string_view message) noexcept { log(LogLevel::Warn, message); }
        void error(std::string_view message) noexcept { log(LogLevel::Error, message); }
        void fatal(std::string_view message) noexcept { log(LogLevel::Fatal, message); }

        /// Flushes every sink.
        void flush() noexcept;

    private:
        struct SinkList;

        void dispatch(const LogMessage& message) noexcept;

        std::string _name;
        std::atomic<LogLevel> _level;
        std::atomic<LogLevel> _flushLevel{LogLevel::Off};
        std::unique_ptr<SinkList> _sinks;
    };

}
