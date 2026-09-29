#pragma once

#include "RST/string/Compare.hpp"
#include "RST/string/Trim.hpp"

#include <charconv>
#include <initializer_list>
#include <optional>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace RST::String {

    template <typename T>
    [[nodiscard]] std::optional<T> parseNumber(std::string_view text, int base = 10) noexcept
    {
        static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, "parseNumber needs a number type");
        if (base < 2 || base > 36 || (std::is_floating_point_v<T> && base != 10))
            return std::nullopt;

        text = trimView(text);
        if (text.size() > 1 && text[0] == '+' && text[1] != '+' && text[1] != '-')
            text.remove_prefix(1);
        if (base == 16 && text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X') && text[2] != '+' && text[2] != '-')
            text.remove_prefix(2);

        const char* const last = text.data() + text.size();
        T value{};
        std::from_chars_result result{};
        if constexpr (std::is_integral_v<T>)
            result = std::from_chars(text.data(), last, value, base);
        else
            result = std::from_chars(text.data(), last, value);

        if (result.ec != std::errc() || result.ptr != last)
            return std::nullopt;
        return value;
    }

    [[nodiscard]] constexpr std::optional<bool> parseBool(std::string_view text) noexcept
    {
        text = trimView(text);
        for (const std::string_view yes : {"true", "yes", "on", "1"})
            if (equalsIgnoreCase(text, yes))
                return true;
        for (const std::string_view no : {"false", "no", "off", "0"})
            if (equalsIgnoreCase(text, no))
                return false;
        return std::nullopt;
    }
}
