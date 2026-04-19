#pragma once
#include "RST/log/LogMessage.hpp"
#include "RST/log/Format/Formatter.hpp"

#include <memory>

namespace RST::Log {

    class ISink {
    public:
        virtual ~ISink() = default;

        virtual void write(const LogMessage& msg) = 0;

        virtual void flush() = 0;

        virtual void setLevel(LogLevel level) = 0;
        virtual LogLevel getLevel() const = 0;
        virtual bool shouldLog(LogLevel lvl) const = 0;

        virtual void setFormatter(std::shared_ptr<Formatter> fmt) = 0;
        virtual void setPattern(std::string_view pattern) = 0;
    };

} // namespace RST::Log
