#include "RST/time/DateTime.hpp"

#include <ctime>

namespace RST::Time {

    namespace {

        [[nodiscard]] std::tm toCalendar(std::time_t time, TimeZone zone) noexcept
        {
            std::tm calendar{};
#if defined(_WIN32)
            if (zone == TimeZone::Utc) {
                ::gmtime_s(&calendar, &time);
            } else {
                ::localtime_s(&calendar, &time);
            }
#else
            if (zone == TimeZone::Utc) {
                ::gmtime_r(&time, &calendar);
            } else {
                ::localtime_r(&time, &calendar);
            }
#endif
            return calendar;
        }

    }

    std::string formatTime(std::chrono::system_clock::time_point time, std::string_view format, TimeZone zone)
    {
        if (format.empty()) {
            return {};
        }
        const std::tm calendar = toCalendar(std::chrono::system_clock::to_time_t(time), zone);
        const std::string pattern(format);

        std::string result;
        for (std::size_t capacity = 64 + 4 * pattern.size(); capacity <= 64 * 1024; capacity *= 4) {
            result.resize(capacity);
            const std::size_t written = std::strftime(result.data(), result.size(), pattern.c_str(), &calendar);
            if (written > 0) {
                result.resize(written);
                return result;
            }
        }
        return {};
    }

    std::string formatNow(std::string_view format, TimeZone zone)
    {
        return formatTime(std::chrono::system_clock::now(), format, zone);
    }
}
