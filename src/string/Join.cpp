#include "RST/string/Join.hpp"

namespace RST::String {

    std::string JoinString(const std::vector<std::string>& stringList, char delimiter) {
        if (stringList.empty())
            return "";

        std::string result;

        for (std::size_t i = 0; i < stringList.size(); i++) {

            result += stringList[i];

            if (delimiter != '\0' && i < stringList.size() - 1)
                result += delimiter;
        }
        return result;
    }
}
