#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace RST::Time {

    enum class TimeZone : std::uint8_t { Local, Utc };

    [[nodiscard]] std::string formatTime(std::chrono::system_clock::time_point time, std::string_view format = "%Y-%m-%d %H:%M:%S", TimeZone zone = TimeZone::Local);

    [[nodiscard]] std::string formatNow(std::string_view format = "%Y-%m-%d %H:%M:%S", TimeZone zone = TimeZone::Local);
}
