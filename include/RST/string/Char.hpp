#pragma once

namespace RST::String {

    /// @name Characters
    /// ASCII only: unlike `<cctype>`, they ignore the locale and accept any `char`.
    /// @{
    [[nodiscard]] constexpr bool isSpace(char c) noexcept { return c == ' ' || (c >= '\t' && c <= '\r'); }
    [[nodiscard]] constexpr bool isDigit(char c) noexcept { return c >= '0' && c <= '9'; }
    [[nodiscard]] constexpr bool isUpper(char c) noexcept { return c >= 'A' && c <= 'Z'; }
    [[nodiscard]] constexpr bool isLower(char c) noexcept { return c >= 'a' && c <= 'z'; }
    [[nodiscard]] constexpr bool isAlpha(char c) noexcept { return isUpper(c) || isLower(c); }
    [[nodiscard]] constexpr bool isAlnum(char c) noexcept { return isAlpha(c) || isDigit(c); }
    [[nodiscard]] constexpr bool isHexDigit(char c) noexcept { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

    [[nodiscard]] constexpr char toLower(char c) noexcept { return isUpper(c) ? static_cast<char>(c + ('a' - 'A')) : c; }
    [[nodiscard]] constexpr char toUpper(char c) noexcept { return isLower(c) ? static_cast<char>(c - ('a' - 'A')) : c; }
    /// @}

}
