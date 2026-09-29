#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string_view>

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

    inline constexpr std::size_t kLogLevelCount = static_cast<std::size_t>(LogLevel::Off) + 1;

    [[nodiscard]] constexpr std::string_view to_string(LogLevel level) noexcept
    {
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

    [[nodiscard]] constexpr std::string_view to_short_string(LogLevel level) noexcept
    {
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

    std::ostream& operator<<(std::ostream& out, LogLevel level);

}
