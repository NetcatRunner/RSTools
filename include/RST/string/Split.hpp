#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace RST::String {

    /// @name Splitting
    /// Every character of `delimiters` separates two parts, and empty parts are skipped.
    /// @{

    /// Splits `str` into strings.
    [[nodiscard]] std::vector<std::string> splitString(std::string_view str, std::string_view delimiters);

    /// Splits `str` into views, without copying: `str` must outlive them.
    [[nodiscard]] std::vector<std::string_view> splitStringView(std::string_view str, std::string_view delimiters);

    /// Splits `str`, keeping the delimiters found between two `quote` characters and removing the quotes.
    [[nodiscard]] std::vector<std::string> splitStringWithQuotes(std::string_view str, std::string_view delimiters, char quote = '\"');
    /// @}
}
