#pragma once

#include <string>

namespace RST::String {

    void ToLower(std::string& str);

    std::string ToLowerCopy(const std::string& str);
}