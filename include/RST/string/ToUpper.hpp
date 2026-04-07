#pragma once

#include <string>

namespace RST::String {

    void ToUpper(std::string& str);

    std::string ToUpperCopy(const std::string& str);
}