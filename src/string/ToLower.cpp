#include "RST/string/ToLower.hpp"

namespace RST::String {

    void toLower(std::string& str) noexcept
    {
        for (char& c : str) {
            c = toLower(c);
        }
    }

    std::string toLowerCopy(std::string_view str)
    {
        std::string result(str);
        toLower(result);
        return result;
    }
}
