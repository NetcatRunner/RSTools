#pragma once

#include <format>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace RST::String {

    /// @name Joining
    /// Numbers are formatted with `std::format`: `join({1, 2, 3}, ", ")` gives `1, 2, 3`.
    /// @{

    /// Joins the elements of `parts`, strings or numbers, with `separator` between them.
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

    /// Joins a braced list, such as `join({4, 5, 6}, "-")`.
    template <typename T>
    [[nodiscard]] std::string join(std::initializer_list<T> parts, std::string_view separator = "")
    {
        return join(std::vector<T>(parts), separator);
    }

    /// Joins `stringList` with a one-character `delimiter`; the default adds none.
    [[nodiscard]] std::string joinString(const std::vector<std::string>& stringList, char delimiter = '\0');
    /// @}
}
