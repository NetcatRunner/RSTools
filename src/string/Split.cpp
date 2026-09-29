#include "RST/string/Split.hpp"

#include <utility>

namespace RST::String {

    std::vector<std::string> splitString(std::string_view str, std::string_view delimiters)
    {
        const std::vector<std::string_view> parts = splitStringView(str, delimiters);
        return std::vector<std::string>(parts.begin(), parts.end());
    }

    std::vector<std::string_view> splitStringView(std::string_view str, std::string_view delimiters)
    {
        std::vector<std::string_view> result;

        std::size_t start = str.find_first_not_of(delimiters);
        while (start != std::string_view::npos) {
            const std::size_t end = str.find_first_of(delimiters, start);
            result.push_back(str.substr(start, end - start));
            start = str.find_first_not_of(delimiters, end);
        }
        return result;
    }

    std::vector<std::string> splitStringWithQuotes(std::string_view str, std::string_view delimiters, char quote)
    {
        std::vector<std::string> result;
        std::string token;
        bool inQuotes = false;
        bool inToken = false;

        for (const char c : str) {
            if (c == quote) {
                inQuotes = !inQuotes;
                inToken = true;
            } else if (!inQuotes && delimiters.find(c) != std::string_view::npos) {
                if (inToken) {
                    result.push_back(std::move(token));
                    token.clear();
                    inToken = false;
                }
            } else {
                token += c;
                inToken = true;
            }
        }
        if (inToken) {
            result.push_back(std::move(token));
        }
        return result;
    }
}
