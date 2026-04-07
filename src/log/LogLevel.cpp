#include "RST/log/LogLevel.hpp"

namespace RST::Log {
    std::ostream& operator<<(std::ostream& out, const RST::Log::LogLevel level) {
        switch (level) {
            case Log::LogLevel::Trace:
                return out << "TRACE";
            case Log::LogLevel::Debug:
                return out << "DEBUG";
            case Log::LogLevel::Info:
                return out << "INFO";
            case Log::LogLevel::Warn:
                return out << "WARN";
            case Log::LogLevel::Error:
                return out << "ERROR";
            case Log::LogLevel::Fatal:
                return out << "FATAL";
        }
        return out << "UNKNOWN";
    }
}

