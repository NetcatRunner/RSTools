#pragma once

#include <cstdint>
#include <string>

namespace RST::Parser {

    /// Outcome of ArgParser::parse(); `Help` and `Version` mean the user asked for that text.
    enum class ParseStatus : std::uint8_t { Ok, Help, Version, Error };

    /// Result of ArgParser::parse(), true when the arguments are valid.
    struct ParseResult {
        ParseStatus status = ParseStatus::Ok;
        /// Text to print for help, version and errors.
        std::string message;

        [[nodiscard]] explicit operator bool() const noexcept { return status == ParseStatus::Ok; }
        /// Exit code for `main`: 2 for errors, 0 otherwise.
        [[nodiscard]] int exitCode() const noexcept { return status == ParseStatus::Error ? 2 : 0; }

        /// Prints the message, to `stderr` for errors and `stdout` otherwise.
        void print() const;
    };
}
