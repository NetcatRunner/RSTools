#pragma once

#include "../LogMessage.hpp"

#include <string>
#include <optional>
#include <functional>
#include <unordered_map>
#include <vector>

namespace RST::Log {

    class Formatter {
    private:
        struct Token {
            enum class Type {
                Literal,
                Year, Month, Day,
                Hour, Minute, Second, Millisecond,
                LevelFull, LevelShort,
                LoggerName,
                Message,
                SourceFile, SourceFunc, SourceLine,
                ThreadId, ProcessId,
            };
            Type type;
            std::optional<std::string> literal;
        };

        void compile(std::string_view pattern);
        static void pad(std::string& dest, int n, size_t width);

        std::string _pattern;
        std::vector<Token> _tokens;

        static const std::unordered_map<Token::Type, std::function<void(const LogMessage& msg, const std::tm& t, int ms, std::string& dest)>> g_formatters;
    public:
        static constexpr std::string_view DEFAULT_PATTERN = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v";

        Formatter(std::string_view pattern = DEFAULT_PATTERN);

        void format(const LogMessage& msg, std::string& dest) const;
        std::string format(const LogMessage& msg) const;

        void setPattern(std::string_view pattern);
        const std::string& getPattern() const { return _pattern; };
    };
}

