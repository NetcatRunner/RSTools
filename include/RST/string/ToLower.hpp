#pragma once

#include "RST/string/Char.hpp"

#include <string>
#include <string_view>

namespace RST::String {

    void toLower(std::string& str) noexcept;

    [[nodiscard]] std::string toLowerCopy(std::string_view str);
}
