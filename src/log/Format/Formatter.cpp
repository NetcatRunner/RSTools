#include "RST/log/Format/Formatter.hpp"
#include "RST/log/LogLevel.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>      // getpid (POSIX) / _getpid (Windows)

#ifdef _WIN32
  #include <process.h>
  #define RST_GETPID() _getpid()
#else
  #include <unistd.h>
  #define RST_GETPID() getpid()
#endif

namespace RST::Log {

void Formatter::pad(std::string& dest, int n, size_t width) {
    std::string num_str = std::to_string(n);

    if (num_str.length() < width) {
        dest.append(width - num_str.length(), '0'); 
    }
    
    dest += num_str;
}

const std::unordered_map<Formatter::Token::Type, std::function<void(const LogMessage& msg, const std::tm& t, int ms, std::string& dest)>> Formatter::g_formatters = {
    {Token::Type::Year, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_year + 1900, 4); }},
    {Token::Type::Month, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_mon + 1, 2); }},
    {Token::Type::Day, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_mday, 2); }},
    {Token::Type::Hour, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_hour, 2); }},
    {Token::Type::Minute, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_min, 2); }},
    {Token::Type::Second, [](const LogMessage&, const std::tm& t, int, std::string& dest) { Formatter::pad(dest, t.tm_sec, 2); }},
    {Token::Type::Millisecond, [](const LogMessage&, const std::tm&, int ms, std::string& dest) { Formatter::pad(dest, ms, 3); }},
    {Token::Type::LevelFull, [](const LogMessage& msg, const std::tm&, int, std::string& dest) { dest += to_string(msg.level); }},
    {Token::Type::LevelShort, [](const LogMessage& msg, const std::tm&, int, std::string& dest) { dest += to_short_string(msg.level); }},
    {Token::Type::LoggerName, [](const LogMessage& msg, const std::tm&, int, std::string& dest) { dest += msg.category; }},
    {Token::Type::Message, [](const LogMessage& msg, const std::tm&, int, std::string& dest) { dest += msg.message; }},
    {Token::Type::SourceFile, [](const LogMessage& msg, const std::tm&, int, std::string& dest) {
        if (!msg.source.has_value() || !msg.source->valid())
            return;
        if (!msg.source->file)
            return;
        std::string_view sv(msg.source->file);
        auto pos = sv.find_last_of("/\\");
        dest += (pos == std::string_view::npos) ? sv : sv.substr(pos + 1);
    }},
    {Token::Type::SourceFunc, [](const LogMessage& msg, const std::tm&, int, std::string& dest) {
        if (msg.source.has_value() && msg.source->valid())
            dest += msg.source->func;
    }},
    {Token::Type::SourceLine, [](const LogMessage& msg, const std::tm&, int, std::string& dest) {
        if (msg.source.has_value() && msg.source->valid())
            dest += std::to_string(msg.source->line);}},
    {Token::Type::ProcessId, [](const LogMessage&, const std::tm&, int, std::string& dest) { dest += std::to_string(RST_GETPID()); }},
};

Formatter::Formatter(std::string_view pattern) {
    compile(pattern);
}

void Formatter::setPattern(std::string_view pattern) {
    compile(pattern);
}

void Formatter::compile(std::string_view pattern) {
    _pattern = std::string(pattern);
    _tokens.clear();

    std::string str;
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != '%' || i + 1 >= pattern.size()) {
            str += pattern[i];
            continue;
        }

        if (!str.empty()) {
            _tokens.push_back({ Token::Type::Literal, std::move(str) });
            str.clear();
        }

        const char spec = pattern[++i];
        Token::Type kind;

        switch (spec) {
            case 'Y': kind = Token::Type::Year;         break;
            case 'm': kind = Token::Type::Month;        break;
            case 'd': kind = Token::Type::Day;          break;
            case 'H': kind = Token::Type::Hour;         break;
            case 'M': kind = Token::Type::Minute;       break;
            case 'S': kind = Token::Type::Second;       break;
            case 'e': kind = Token::Type::Millisecond;  break;
            case 'l': kind = Token::Type::LevelFull;    break;
            case 'L': kind = Token::Type::LevelShort;   break;
            case 'n': kind = Token::Type::LoggerName;   break;
            case 'v': kind = Token::Type::Message;      break;
            case 'f': kind = Token::Type::SourceFile;   break;
            case 'F': kind = Token::Type::SourceFunc;   break;
            case '#': kind = Token::Type::SourceLine;   break;
            case 'P': kind = Token::Type::ProcessId;    break;
            case '%': str += '%'; continue;
            default:  str += '%'; str += spec; continue;
        }
        _tokens.push_back({ kind, {} });
    }

    if (!str.empty())
        _tokens.push_back({ Token::Type::Literal, std::move(str) });
}

void Formatter::format(const LogMessage& msg, std::string& dest) const {
    const auto tt = std::chrono::system_clock::to_time_t(msg.time);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(msg.time.time_since_epoch()) % 1000;
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &tt);
#else
    localtime_r(&tt, &tm_buf);
#endif
    const std::tm& t = tm_buf;

    for (const auto& tok : _tokens) {
        if (tok.type == Token::Type::Literal) {
            if (tok.literal.has_value()) {
                dest += tok.literal.value();
            }
            continue;
        }
        auto it = g_formatters.find(tok.type);
        if (it != g_formatters.end()) {
            it->second(msg, t, static_cast<int>(ms.count()), dest);
        }
    }
}

std::string Formatter::format(const LogMessage& msg) const {
    std::string out;
    format(msg, out);
    return out;
}

} // namespace RST::Log
