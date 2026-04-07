#include "RST/string/Trim.hpp"

#include <algorithm>

namespace RST::String {

    void Trim(std::string& str) {
        str.erase(0,str.find_first_not_of(" \n\r\t"));
        str.erase(str.find_last_not_of(" \n\r\t")+1);
    }

    void Rtrim(std::string& str) {
        str.erase(str.find_last_not_of(" \n\r\t")+1);
    }
    void Ltrim(std::string& str) {
        str.erase(0,str.find_first_not_of(" \n\r\t"));
    }

    std::string TrimCopy(const std::string& str) {
        std::string s = str;
        s.erase(0,s.find_first_not_of(" \n\r\t"));
        s.erase(s.find_last_not_of(" \n\r\t")+1);
        return s;
    }

    std::string RtrimCopy(const std::string& str) {
        std::string s = str;
        s.erase(s.find_last_not_of(" \n\r\t")+1);
        return s;
    }
    std::string LtrimCopy(const std::string& str) {
        std::string s = str;
        s.erase(0,s.find_first_not_of(" \n\r\t"));
        return s;
    }
}