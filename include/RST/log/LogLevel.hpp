#pragma once

#include <iostream>
#include <cstdint>

namespace RST::Log {

    enum class LogLevel : std::uint8_t {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
        Off
    };

std::ostream&    operator<<(std::ostream& out, LogLevel level);
std::string_view to_string(LogLevel level);       // "TRACE", "DEBUG"…
std::string_view to_short_string(LogLevel level); // "TRC", "DBG"…
}
