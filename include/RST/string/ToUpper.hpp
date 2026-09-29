#pragma once

#include "RST/string/Char.hpp"

#include <string>
#include <string_view>

namespace RST::String {

    void toUpper(std::string& str) noexcept;

    [[nodiscard]] std::string toUpperCopy(std::string_view str);
}
