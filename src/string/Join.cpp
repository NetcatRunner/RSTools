#include "RST/string/Join.hpp"

namespace RST::String {

    std::string joinString(const std::vector<std::string>& stringList, char delimiter)
    {
        return join(stringList, delimiter == '\0' ? std::string_view() : std::string_view(&delimiter, 1));
    }
}
