#pragma once

#include <format>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace RST::String {

    template <typename Container>
    [[nodiscard]] std::string join(const Container& parts, std::string_view separator = "")
    {
        std::string result;
        bool first = true;
        for (const auto& part : parts) {
            if (!first)
                result += separator;
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(part)>>)
                result += std::format("{}", part);
            else
                result += part;
            first = false;
        }
        return result;
    }

    template <typename T>
    [[nodiscard]] std::string join(std::initializer_list<T> parts, std::string_view separator = "")
    {
        return join(std::vector<T>(parts), separator);
    }

    [[nodiscard]] std::string joinString(const std::vector<std::string>& stringList, char delimiter = '\0');
}
