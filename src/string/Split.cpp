#include "RST/string/Split.hpp"

namespace RST::String {

    std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters) {
        std::vector<std::string> result;

        std::size_t start = str.find_first_not_of(delimiters, 0);

        while (start != std::string::npos) {
            std::size_t end = str.find_first_of(delimiters, start);

            result.push_back(str.substr(start, end - start));

            start = str.find_first_not_of(delimiters, end);
        }
        
        return result;
    }
}