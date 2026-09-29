#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace RST::String {

    [[nodiscard]] std::vector<std::string> splitString(std::string_view str, std::string_view delimiters);

    [[nodiscard]] std::vector<std::string_view> splitStringView(std::string_view str, std::string_view delimiters);

    [[nodiscard]] std::vector<std::string> splitStringWithQuotes(std::string_view str, std::string_view delimiters, char quote = '\"');
}
