#include "RST/log/LogLevel.hpp"

namespace RST::Log {
    std::ostream& operator<<(std::ostream& out, const LogLevel level) {
        return out << to_string(level);
    }

    std::string_view to_string(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            case LogLevel::Off:   return "OFF";
        }
        return "UNKNOWN";
    }

    std::string_view to_short_string(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRC";
            case LogLevel::Debug: return "DBG";
            case LogLevel::Info:  return "INF";
            case LogLevel::Warn:  return "WRN";
            case LogLevel::Error: return "ERR";
            case LogLevel::Fatal: return "FTL";
            case LogLevel::Off:   return "OFF";
        }
        return "???";
    }

}

