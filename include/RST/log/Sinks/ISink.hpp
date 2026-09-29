#pragma once

#include "RST/log/LogLevel.hpp"

#include <memory>
#include <string_view>

namespace RST::Log {

    struct LogMessage;
    class Formatter;

    class ISink {
    public:
        virtual ~ISink() = default;

        virtual void write(const LogMessage& message) = 0;
        virtual void flush() = 0;

        virtual void setLevel(LogLevel level) noexcept = 0;
        [[nodiscard]] virtual LogLevel getLevel() const noexcept = 0;
        [[nodiscard]] virtual bool shouldLog(LogLevel level) const noexcept = 0;

        virtual void setFormatter(std::shared_ptr<Formatter> formatter) = 0;
        virtual void setPattern(std::string_view pattern) = 0;
    };

}
