#pragma once

#include "ISink.hpp"

namespace RST::Log {

    class ASink : public ISink {
    protected:
        virtual void log(const LogMessage& msg) = 0;

        std::string formatted(const LogMessage& msg) const {
            std::string out;
            _formatter->format(msg, out);
            return out;
        }

        std::shared_ptr<Formatter> _formatter = std::make_shared<Formatter>();
        LogLevel _minLevel  = LogLevel::Trace;
    public:
        virtual ~ASink() = default;

        void write(const LogMessage& msg) override {
            if (!shouldLog(msg.level))
                return;
            log(msg);
        };

        virtual void flush() override {};

        void setLevel(LogLevel level) override { _minLevel = level; };
        LogLevel getLevel() const override { return _minLevel; };
        bool shouldLog(LogLevel lvl) const override { return lvl >= _minLevel; };

        void setFormatter(std::shared_ptr<Formatter> fmt) override {_formatter = std::move(fmt);};
        void setPattern(std::string_view pattern) override {setFormatter(std::make_shared<Formatter>(pattern));};
    };
}
