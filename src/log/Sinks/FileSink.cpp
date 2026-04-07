#include "RST/log/Sinks/FileSink.hpp"

#include "RST/log/LogLevel.hpp"
#include "RST/log/LogMessage.hpp"

#include <chrono>
#include <iomanip>

namespace RST::Log {
    void Log::FileSink::log(const LogMessage& msg)
    {
        auto time = std::chrono::system_clock::to_time_t(msg.time);
    
        m_File << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "]"
               << "[" << msg.level << "]"
               << "[" << msg.category << "] "
               << msg.message << std::endl;
    }
}
