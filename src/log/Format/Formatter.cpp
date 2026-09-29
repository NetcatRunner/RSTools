#include "RST/log/Format/Formatter.hpp"

#include "RST/log/LogMessage.hpp"

#include "log/detail/Platform.hpp"

#include <array>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <limits>

namespace RST::Log {

    namespace {

        void appendNumber(std::string& dest, std::uint64_t value, std::size_t width = 0)
        {
            std::array<char, 24> digits{};
            const std::to_chars_result result = std::to_chars(digits.data(), digits.data() + digits.size(), value);
            const auto length = static_cast<std::size_t>(result.ptr - digits.data());
            if (length < width) {
                dest.append(width - length, '0');
            }
            dest.append(digits.data(), length);
        }

        void appendPadded(std::string& dest, int value, std::size_t width)
        {
            appendNumber(dest, static_cast<std::uint64_t>(value < 0 ? 0 : value), width);
        }

        [[nodiscard]] std::string_view fileName(const char* path) noexcept
        {
            const std::string_view full(path);
            const std::size_t slash = full.find_last_of("/\\");
            return slash == std::string_view::npos ? full : full.substr(slash + 1);
        }

        [[nodiscard]] const std::tm& calendarTime(std::time_t seconds) noexcept
        {
            struct Cache {
                std::time_t seconds = std::numeric_limits<std::time_t>::min();
                std::tm calendar{};
            };
            thread_local Cache cache;
            if (cache.seconds != seconds) {
                cache.calendar = detail::localTime(seconds);
                cache.seconds = seconds;
            }
            return cache.calendar;
        }

    }

    Formatter::Formatter(std::string_view pattern)
    {
        compile(pattern);
    }

    void Formatter::setPattern(std::string_view pattern)
    {
        compile(pattern);
    }

    void Formatter::compile(std::string_view pattern)
    {
        _pattern = std::string(pattern);
        _pieces.clear();
        _needsTime = false;

        std::string literal;
        const auto flushLiteral = [&] {
            if (!literal.empty()) {
                _pieces.push_back({Token::Literal, std::move(literal)});
                literal.clear();
            }
        };

        for (std::size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] != '%' || i + 1 >= pattern.size()) {
                literal += pattern[i];
                continue;
            }

            const char spec = pattern[++i];
            Token token = Token::Literal;
            switch (spec) {
                case 'Y': token = Token::Year;        break;
                case 'm': token = Token::Month;       break;
                case 'd': token = Token::Day;         break;
                case 'H': token = Token::Hour;        break;
                case 'M': token = Token::Minute;      break;
                case 'S': token = Token::Second;      break;
                case 'e': token = Token::Millisecond; break;
                case 'l': token = Token::LevelFull;   break;
                case 'L': token = Token::LevelShort;  break;
                case 'n': token = Token::Category;    break;
                case 'v': token = Token::Message;     break;
                case 'f': token = Token::SourceFile;  break;
                case 'F': token = Token::SourceFunc;  break;
                case '#': token = Token::SourceLine;  break;
                case 't': token = Token::ThreadId;    break;
                case 'P': token = Token::ProcessId;   break;
                case '%':
                    literal += '%';
                    continue;
                default:
                    literal += '%';
                    literal += spec;
                    continue;
            }

            flushLiteral();
            _pieces.push_back({token, {}});
            _needsTime = _needsTime || (token >= Token::Year && token <= Token::Millisecond);
        }
        flushLiteral();
    }

    void Formatter::format(const LogMessage& message, std::string& dest) const
    {
        const std::tm* calendar = nullptr;
        std::uint64_t milliseconds = 0;
        if (_needsTime) {
            const auto sinceEpoch = message.time.time_since_epoch();
            const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch);
            calendar = &calendarTime(static_cast<std::time_t>(seconds.count()));
            milliseconds = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch - seconds).count());
        }

        for (const Piece& piece : _pieces) {
            switch (piece.token) {
                case Token::Literal:     dest += piece.literal; break;
                case Token::Year:        appendPadded(dest, calendar->tm_year + 1900, 4); break;
                case Token::Month:       appendPadded(dest, calendar->tm_mon + 1, 2); break;
                case Token::Day:         appendPadded(dest, calendar->tm_mday, 2); break;
                case Token::Hour:        appendPadded(dest, calendar->tm_hour, 2); break;
                case Token::Minute:      appendPadded(dest, calendar->tm_min, 2); break;
                case Token::Second:      appendPadded(dest, calendar->tm_sec, 2); break;
                case Token::Millisecond: appendNumber(dest, milliseconds, 3); break;
                case Token::LevelFull:   dest += toString(message.level); break;
                case Token::LevelShort:  dest += toShortString(message.level); break;
                case Token::Category:    dest += message.category; break;
                case Token::Message:     dest += message.message; break;
                case Token::SourceFile:
                    if (message.source.valid()) {
                        dest += fileName(message.source.file);
                    }
                    break;
                case Token::SourceFunc:
                    if (message.source.valid() && message.source.func != nullptr) {
                        dest += message.source.func;
                    }
                    break;
                case Token::SourceLine:
                    if (message.source.valid()) {
                        appendPadded(dest, message.source.line, 0);
                    }
                    break;
                case Token::ThreadId:    appendNumber(dest, message.threadId); break;
                case Token::ProcessId:   appendNumber(dest, detail::currentProcessId()); break;
            }
        }
    }

    std::string Formatter::format(const LogMessage& message) const
    {
        std::string out;
        format(message, out);
        return out;
    }

}
