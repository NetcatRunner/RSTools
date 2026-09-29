#include "RST/string/Replace.hpp"

#include <utility>

namespace RST::String {

    namespace {

        std::size_t replaceInto(std::string& out, std::string_view str, std::string_view from, std::string_view to, std::size_t match)
        {
            out.reserve(str.size() + (to.size() > from.size() ? to.size() - from.size() : 0));

            std::size_t count = 0;
            std::size_t start = 0;
            while (match != std::string_view::npos) {
                out.append(str.substr(start, match - start));
                out.append(to);
                ++count;
                start = match + from.size();
                match = str.find(from, start);
            }
            out.append(str.substr(start));
            return count;
        }

    }

    std::size_t replaceAll(std::string& str, std::string_view from, std::string_view to)
    {
        const std::size_t match = from.empty() ? std::string::npos : str.find(from);
        if (match == std::string::npos) {
            return 0;
        }
        std::string result;
        const std::size_t count = replaceInto(result, str, from, to, match);
        str = std::move(result);
        return count;
    }

    std::string replaceAllCopy(std::string_view str, std::string_view from, std::string_view to)
    {
        const std::size_t match = from.empty() ? std::string_view::npos : str.find(from);
        if (match == std::string_view::npos) {
            return std::string(str);
        }
        std::string result;
        replaceInto(result, str, from, to, match);
        return result;
    }

    bool replaceFirst(std::string& str, std::string_view from, std::string_view to)
    {
        const std::size_t match = from.empty() ? std::string::npos : str.find(from);
        if (match == std::string::npos) {
            return false;
        }
        str.replace(match, from.size(), to);
        return true;
    }
}
