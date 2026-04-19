#pragma once

#include <vector>
#include <string>

namespace RST::String {

    std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter);
    std::vector<std::string> SplitStringWithQuotes(const std::string& str, const std::string& delimiters, char quote = '\"');
}
