#pragma once

#include "ASink.hpp"

#include <unordered_map>
#include <string>

#include "../LogLevel.hpp"

namespace RST::Log {

    class ConsoleSink  : public ASink
    {
    private:
        std::unordered_map<LogLevel, std::string> _colors;
    public:
        ConsoleSink();

        void log(const LogMessage& msg) override;
        void setColor(LogLevel level, const std::string& color_code);
    };

}
