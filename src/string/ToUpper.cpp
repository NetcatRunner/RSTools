#include "RST/string/ToUpper.hpp"

#include <algorithm>

namespace RST::String {

    void ToUpper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
    
    std::string ToUpperCopy(const std::string& str) {
        std::string s = str;
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    }
}