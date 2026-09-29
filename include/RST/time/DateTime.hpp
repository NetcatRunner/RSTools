#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace RST::Time {

    /// Time zone used to format dates.
    enum class TimeZone : std::uint8_t { Local, Utc };

    /// Formats `time` with `std::strftime` codes, such as `%Y-%m-%d %H:%M:%S`; returns an empty string on failure.
    [[nodiscard]] std::string formatTime(std::chrono::system_clock::time_point time, std::string_view format = "%Y-%m-%d %H:%M:%S", TimeZone zone = TimeZone::Local);

    /// Formats the current time; see formatTime().
    [[nodiscard]] std::string formatNow(std::string_view format = "%Y-%m-%d %H:%M:%S", TimeZone zone = TimeZone::Local);
}
