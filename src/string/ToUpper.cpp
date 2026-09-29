#include "RST/string/ToUpper.hpp"

namespace RST::String {

    void toUpper(std::string& str) noexcept
    {
        for (char& c : str) {
            c = toUpper(c);
        }
    }

    std::string toUpperCopy(std::string_view str)
    {
        std::string result(str);
        toUpper(result);
        return result;
    }
}
