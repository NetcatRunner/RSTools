#pragma once

#include "RST/string/Char.hpp"

#include <string>
#include <string_view>

namespace RST::String {

    /// @name Case conversion
    /// @{

    /// Uppercases the ASCII letters of `str` in place.
    void toUpper(std::string& str) noexcept;

    /// Returns a copy of `str` with its ASCII letters uppercased.
    [[nodiscard]] std::string toUpperCopy(std::string_view str);
    /// @}
}
