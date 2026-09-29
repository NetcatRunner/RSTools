#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace RST::String {

    std::size_t replaceAll(std::string& str, std::string_view from, std::string_view to);

    [[nodiscard]] std::string replaceAllCopy(std::string_view str, std::string_view from, std::string_view to);

    bool replaceFirst(std::string& str, std::string_view from, std::string_view to);
}
