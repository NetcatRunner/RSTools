#pragma once

#include <string>

namespace RST::String {

    void Trim(std::string& str);

    void Rtrim(std::string& str);
    void Ltrim(std::string& str);

    std::string TrimCopy(const std::string& str);

    std::string RtrimCopy(const std::string& str);
    std::string LtrimCopy(const std::string& str);
}