#pragma once

#include <vector>
#include <string>

namespace RST::String {

    std::string JoinString(const std::vector<std::string>& stringList, char delimiter = '\0');
}