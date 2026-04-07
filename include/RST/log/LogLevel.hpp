#pragma once

#include <iostream>

namespace RST::Log {
    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    std::ostream& operator<<(std::ostream& out, const RST::Log::LogLevel level);
}
