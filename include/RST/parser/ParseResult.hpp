#pragma once

#include <cstdint>
#include <string>

namespace RST::Parser {

    enum class ParseStatus : std::uint8_t { Ok, Help, Version, Error };

    struct ParseResult {
        ParseStatus status = ParseStatus::Ok;
        std::string message;

        [[nodiscard]] explicit operator bool() const noexcept { return status == ParseStatus::Ok; }
        [[nodiscard]] int exitCode() const noexcept { return status == ParseStatus::Error ? 2 : 0; }

        void print() const;
    };
}
