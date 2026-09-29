#pragma once

#include "RST/string/Char.hpp"

#include <cstddef>
#include <string_view>

namespace RST::String {
    [[nodiscard]] constexpr bool contains(std::string_view str, std::string_view needle) noexcept { return str.find(needle) != std::string_view::npos; }
    [[nodiscard]] constexpr bool contains(std::string_view str, char c) noexcept { return str.find(c) != std::string_view::npos; }

    [[nodiscard]] constexpr bool equalsIgnoreCase(std::string_view lhs, std::string_view rhs) noexcept
    {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        for (std::size_t i = 0; i < lhs.size(); ++i) {
            if (toLower(lhs[i]) != toLower(rhs[i])) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr bool startsWithIgnoreCase(std::string_view str, std::string_view prefix) noexcept
    {
        return str.size() >= prefix.size() && equalsIgnoreCase(str.substr(0, prefix.size()), prefix);
    }

    [[nodiscard]] constexpr bool endsWithIgnoreCase(std::string_view str, std::string_view suffix) noexcept
    {
        return str.size() >= suffix.size() && equalsIgnoreCase(str.substr(str.size() - suffix.size()), suffix);
    }

    [[nodiscard]] constexpr std::size_t findIgnoreCase(std::string_view str, std::string_view needle, std::size_t from = 0) noexcept
    {
        if (from > str.size() || needle.size() > str.size() - from) {
            return std::string_view::npos;
        }
        for (std::size_t i = from; i <= str.size() - needle.size(); ++i) {
            if (equalsIgnoreCase(str.substr(i, needle.size()), needle)) {
                return i;
            }
        }
        return std::string_view::npos;
    }

    [[nodiscard]] constexpr bool containsIgnoreCase(std::string_view str, std::string_view needle) noexcept
    {
        return findIgnoreCase(str, needle) != std::string_view::npos;
    }

    [[nodiscard]] constexpr int compareIgnoreCase(std::string_view lhs, std::string_view rhs) noexcept
    {
        const std::size_t common = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
        for (std::size_t i = 0; i < common; ++i) {
            const auto a = static_cast<unsigned char>(toLower(lhs[i]));
            const auto b = static_cast<unsigned char>(toLower(rhs[i]));
            if (a != b) {
                return a < b ? -1 : 1;
            }
        }
        return lhs.size() == rhs.size() ? 0 : (lhs.size() < rhs.size() ? -1 : 1);
    }
}
