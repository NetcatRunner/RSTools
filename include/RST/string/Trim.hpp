#pragma once

#include <string>
#include <string_view>

namespace RST::String {

    inline constexpr std::string_view Whitespace = " \t\n\r\f\v";

    [[nodiscard]] constexpr std::string_view ltrimView(std::string_view str, std::string_view chars = Whitespace) noexcept
    {
        const std::size_t first = str.find_first_not_of(chars);
        str.remove_prefix(first == std::string_view::npos ? str.size() : first);
        return str;
    }

    [[nodiscard]] constexpr std::string_view rtrimView(std::string_view str, std::string_view chars = Whitespace) noexcept
    {
        const std::size_t last = str.find_last_not_of(chars);
        str.remove_suffix(last == std::string_view::npos ? str.size() : str.size() - last - 1);
        return str;
    }

    [[nodiscard]] constexpr std::string_view trimView(std::string_view str, std::string_view chars = Whitespace) noexcept
    {
        return rtrimView(ltrimView(str, chars), chars);
    }

    // ── In place ─────────────────────────────────────────────────────────────
    void trim(std::string& str, std::string_view chars = Whitespace);
    void ltrim(std::string& str, std::string_view chars = Whitespace);
    void rtrim(std::string& str, std::string_view chars = Whitespace);

    // ── Copies ─────────────────────
    [[nodiscard]] std::string trimCopy(std::string_view str, std::string_view chars = Whitespace);
    [[nodiscard]] std::string ltrimCopy(std::string_view str, std::string_view chars = Whitespace);
    [[nodiscard]] std::string rtrimCopy(std::string_view str, std::string_view chars = Whitespace);
}
