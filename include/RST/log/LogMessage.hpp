#pragma once


#include <iostream>
#include <chrono>
#include <string>
#include "LogLevel.hpp"

namespace RST::Log {

    struct SourceLocation {
        const char* file = nullptr;
        const char* func = nullptr;
        int         line = 0;

        constexpr bool valid() const noexcept { return file != nullptr; }
    };

    #define RST_SOURCE_LOCATION \
        RST::Log::SourceLocation { __FILE__, __func__, __LINE__ }

    struct LogMessage {
        std::chrono::system_clock::time_point time;
        LogLevel                              level    = LogLevel::Info;
        std::string                           category;
        std::string                           message;
        SourceLocation                        source;

        LogMessage() = default;

        LogMessage(LogLevel lvl, std::string cat, std::string msg, SourceLocation src = {})
            : time(std::chrono::system_clock::now()), level(lvl), category(std::move(cat)), message(std::move(msg)), source(src) {}
    };
}
