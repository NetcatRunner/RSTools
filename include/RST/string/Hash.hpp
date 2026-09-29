#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace RST::String {

    [[nodiscard]] constexpr std::uint64_t hash(std::string_view str) noexcept
    {
        std::uint64_t hash = 0xcbf29ce484222325ull;
        for (const char c : str) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 0x100000001b3ull;
        }
        return hash;
    }

    namespace Literals {

        [[nodiscard]] consteval std::uint64_t operator""_hash(const char* str, std::size_t size) noexcept
        {
            return hash(std::string_view(str, size));
        }
    }
}
