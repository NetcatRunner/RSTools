#pragma once

#include "RST/string/Char.hpp"

#include <string>
#include <string_view>

namespace RST::String {

    /// @name Case conversion
    /// @{

    /// Lowercases the ASCII letters of `str` in place.
    void toLower(std::string& str) noexcept;

    /// Returns a copy of `str` with its ASCII letters lowercased.
    [[nodiscard]] std::string toLowerCopy(std::string_view str);
    /// @}
}
