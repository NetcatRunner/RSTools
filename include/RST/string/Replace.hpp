#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace RST::String {

    /// @name Replacing
    /// An empty `from` matches nothing.
    /// @{

    /// Replaces every `from` in `str` with `to` and returns the number of replacements.
    std::size_t replaceAll(std::string& str, std::string_view from, std::string_view to);

    /// Returns a copy of `str` with every `from` replaced by `to`.
    [[nodiscard]] std::string replaceAllCopy(std::string_view str, std::string_view from, std::string_view to);

    /// Replaces the first `from` in `str` with `to`; returns false if there is none.
    bool replaceFirst(std::string& str, std::string_view from, std::string_view to);
    /// @}
}
