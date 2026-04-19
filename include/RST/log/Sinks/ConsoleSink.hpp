#pragma once

#include "ASink.hpp"

#include <unordered_map>
#include <string>

namespace RST::Log {

    class ConsoleSink : public ASink {    
    private:
        std::ostream& _target;
        bool _colorEnabled;
        std::unordered_map<LogLevel, std::string> _colors;
    protected:
        void log(const LogMessage& msg) override;
    public:
        ConsoleSink(std::ostream& target = std::cout);

        void setColor(LogLevel level, std::string_view color_code);

        void setColorEnabled(bool enabled) noexcept { _colorEnabled = enabled; }

        void flush() override { _target.flush(); }

    };

}
