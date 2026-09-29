#pragma once

#include "RST/log/LogMessage.hpp"
#include "RST/log/Sinks/ISink.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

namespace RST::Log {

    /// Base class for sinks: handles the level, the pattern and the locking.
    ///
    /// Subclasses only implement log(), and flushSink() if they buffer their output.
    class ASink : public ISink {
    public:
        ASink();
        ~ASink() override;

        void write(const LogMessage& message) final;
        void flush() final;

        void setLevel(LogLevel level) noexcept final { _level.store(level, std::memory_order_relaxed); }
        [[nodiscard]] LogLevel getLevel() const noexcept final { return _level.load(std::memory_order_relaxed); }
        [[nodiscard]] bool shouldLog(LogLevel level) const noexcept final { return level >= getLevel(); }

        void setFormatter(std::shared_ptr<Formatter> formatter) final;
        void setPattern(std::string_view pattern) final;

    protected:
        /// Writes one message; called with the sink mutex locked.
        virtual void log(const LogMessage& message) = 0;
        /// Flushes the output; called with the sink mutex locked.
        virtual void flushSink() {}

        /// The message formatted with the sink pattern.
        [[nodiscard]] std::string_view formatted(const LogMessage& message);
        [[nodiscard]] std::mutex& sinkMutex() noexcept { return _mutex; }

    private:
        std::mutex _mutex;
        std::shared_ptr<Formatter> _formatter;
        std::string _line;
        std::atomic<LogLevel> _level{LogLevel::Trace};
    };

}
