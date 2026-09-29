#include "RST/string/Trim.hpp"

namespace RST::String {

    void trim(std::string& str, std::string_view chars)
    {
        rtrim(str, chars);
        ltrim(str, chars);
    }

    void ltrim(std::string& str, std::string_view chars)
    {
        str.erase(0, str.find_first_not_of(chars));
    }

    void rtrim(std::string& str, std::string_view chars)
    {
        str.erase(str.find_last_not_of(chars) + 1);
    }

    std::string trimCopy(std::string_view str, std::string_view chars)
    {
        return std::string(trimView(str, chars));
    }

    std::string ltrimCopy(std::string_view str, std::string_view chars)
    {
        return std::string(ltrimView(str, chars));
    }

    std::string rtrimCopy(std::string_view str, std::string_view chars)
    {
        return std::string(rtrimView(str, chars));
    }
}
