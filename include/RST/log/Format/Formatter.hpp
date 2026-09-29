#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace RST::Log {

    struct LogMessage;

    class Formatter {
    public:
        static constexpr std::string_view DEFAULT_PATTERN = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v";

        explicit Formatter(std::string_view pattern = DEFAULT_PATTERN);

        void format(const LogMessage& message, std::string& dest) const;
        [[nodiscard]] std::string format(const LogMessage& message) const;

        void setPattern(std::string_view pattern);
        [[nodiscard]] const std::string& getPattern() const noexcept { return _pattern; }

    private:
        enum class Token : std::uint8_t {
            Literal,
            Year, Month, Day,
            Hour, Minute, Second, Millisecond,
            LevelFull, LevelShort,
            Category,
            Message,
            SourceFile, SourceFunc, SourceLine,
            ThreadId, ProcessId,
        };

        struct Piece {
            Token token = Token::Literal;
            std::string literal;
        };

        void compile(std::string_view pattern);

        std::string _pattern;
        std::vector<Piece> _pieces;
        bool _needsTime = false;
    };

}
