#pragma once

#include <cstdlib>
#include <string_view>

namespace RST::Log::Terminal
{

inline constexpr const char* RESET     = "\033[0m";
inline constexpr const char* BOLD      = "\033[1m";
inline constexpr const char* DIM       = "\033[2m";
inline constexpr const char* ITALIC    = "\033[3m";
inline constexpr const char* UNDERLINE = "\033[4m";

inline constexpr const char* BLACK   = "\033[30m";
inline constexpr const char* RED     = "\033[31m";
inline constexpr const char* GREEN   = "\033[32m";
inline constexpr const char* YELLOW  = "\033[33m";
inline constexpr const char* BLUE    = "\033[34m";
inline constexpr const char* MAGENTA = "\033[35m";
inline constexpr const char* CYAN    = "\033[36m";
inline constexpr const char* WHITE   = "\033[37m";

inline constexpr const char* BRIGHT_RED     = "\033[91m";
inline constexpr const char* BRIGHT_GREEN   = "\033[92m";
inline constexpr const char* BRIGHT_YELLOW  = "\033[93m";
inline constexpr const char* BRIGHT_BLUE    = "\033[94m";
inline constexpr const char* BRIGHT_MAGENTA = "\033[95m";
inline constexpr const char* BRIGHT_CYAN    = "\033[96m";
inline constexpr const char* BRIGHT_WHITE   = "\033[97m";

inline constexpr const char* BG_RED    = "\033[41m";
inline constexpr const char* BG_GREEN  = "\033[42m";
inline constexpr const char* BG_YELLOW = "\033[43m";
inline constexpr const char* BG_BLUE   = "\033[44m";
inline constexpr const char* BG_WHITE  = "\033[47m";

inline bool supportsColor() noexcept {
#ifdef _WIN32
    return false; // SetConsoleMode on Windows 10+
#else
    const char* term = std::getenv("TERM");
    return (term != nullptr && std::string_view(term) != "dumb");
#endif
}
}
