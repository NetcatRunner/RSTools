#pragma once

#include "RST/log/LogLevel.hpp"

#include <memory>
#include <string_view>

namespace RST::Log {

    struct LogMessage;
    class Formatter;

    /// Interface of the log destinations; derive from ASink to write a new sink.
    class ISink {
    public:
        virtual ~ISink() = default;

        /// Receives a message from a logger.
        virtual void write(const LogMessage& message) = 0;
        virtual void flush() = 0;

        /// Minimum level accepted by this sink.
        virtual void setLevel(LogLevel level) noexcept = 0;
        [[nodiscard]] virtual LogLevel getLevel() const noexcept = 0;
        [[nodiscard]] virtual bool shouldLog(LogLevel level) const noexcept = 0;

        /// Shares a Formatter between sinks.
        virtual void setFormatter(std::shared_ptr<Formatter> formatter) = 0;
        /// Formats the messages with `pattern`; see Formatter.
        virtual void setPattern(std::string_view pattern) = 0;
    };

}
