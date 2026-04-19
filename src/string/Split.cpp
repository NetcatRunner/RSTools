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

    std::vector<std::string> SplitStringWithQuotes(const std::string& str, const std::string& delimiters, char quote) {
        std::vector<std::string> result;
        std::string currentToken;
        bool inQuotes = false;
        bool inToken = false;

        for (const char c : str) {
            if (c == quote) {
                inQuotes = !inQuotes;
            } else if (delimiters.find(c) != std::string::npos && !inQuotes) {
                if (inToken) {
                    result.push_back(currentToken);
                    currentToken.clear();
                    inToken = false;
                }
            } else {
                currentToken += c;
                inToken = true;
            }
        }
        if (inToken)
            result.push_back(currentToken);

        return result;
    }
}
