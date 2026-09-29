#pragma once

#include <iosfwd>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace RST::Log {

    class ISink;
    class RingBufferSink;
    struct LogMessage;

    class Logger {
    public:
        explicit Logger(std::string name, LogLevel level = LogLevel::Trace);
        ~Logger();

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        [[nodiscard]] const std::string& getName() const noexcept { return _name; }

        // ── Levels ───────────────────────────────────────────────────────────
        void setLevel(LogLevel level) noexcept { _level.store(level, std::memory_order_relaxed); }
        [[nodiscard]] LogLevel getLevel() const noexcept { return _level.load(std::memory_order_relaxed); }
        [[nodiscard]] bool shouldLog(LogLevel level) const noexcept { return level != LogLevel::Off && level >= getLevel(); }

        // ── Flush ───────────────────────────────────────────────────────────
        void flushOn(LogLevel level) noexcept { _flushLevel.store(level, std::memory_order_relaxed); }
        [[nodiscard]] LogLevel getFlushLevel() const noexcept { return _flushLevel.load(std::memory_order_relaxed); }

        // ── Sinks ────────────────────────────────────────────────────────────
        void addSink(std::shared_ptr<ISink> sink);
        void addSink(std::shared_ptr<ISink> sink, RingBufferSink& history);
        bool removeSink(const std::shared_ptr<ISink>& sink) noexcept;
        void clearSinks() noexcept;
        [[nodiscard]] std::size_t sinkCount() const noexcept;

        // ── Formatted logging ────────────────────────────────────────────────
        template <class... Args>
        void log(LogLevel level, const SourceLocation& source, std::format_string<Args...> format, Args&&... args) noexcept
        {
            if (shouldLog(level)) {
                vlog(level, source, _name, format.get(), std::make_format_args(args...));
            }
        }

        void vlog(LogLevel level, const SourceLocation& source, std::string_view category, std::string_view format, std::format_args args) noexcept;

        template <class... Args> void trace(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Trace, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void debug(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Debug, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void info(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Info, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void warn(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Warn, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void error(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Error, {}, format, std::forward<Args>(args)...); }
        template <class... Args> void fatal(std::format_string<Args...> format, Args&&... args) noexcept { log(LogLevel::Fatal, {}, format, std::forward<Args>(args)...); }

        // ── Pre-formatted text ───────────────────────────────────────────────
        void log(LogLevel level, std::string_view message, const SourceLocation& source = {}) noexcept;

        void trace(std::string_view message) noexcept { log(LogLevel::Trace, message); }
        void debug(std::string_view message) noexcept { log(LogLevel::Debug, message); }
        void info(std::string_view message) noexcept { log(LogLevel::Info, message); }
        void warn(std::string_view message) noexcept { log(LogLevel::Warn, message); }
        void error(std::string_view message) noexcept { log(LogLevel::Error, message); }
        void fatal(std::string_view message) noexcept { log(LogLevel::Fatal, message); }

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
