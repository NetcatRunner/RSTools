#include "RST/string/ToLower.hpp"

#include <algorithm>

namespace RST::String {

    void ToLower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    
    std::string ToLowerCopy(const std::string& str) {
        std::string s = str;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
}