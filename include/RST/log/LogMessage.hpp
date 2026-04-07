#pragma once


#include <iostream>

#include <chrono>
#include <string>
#include "LogLevel.hpp"

namespace RST::Log {

    struct LogMessage {
        LogLevel level;
        std::string message;
        std::string category;

        std::chrono::system_clock::time_point time;
    };

}
