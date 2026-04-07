#include "RST/log/Sinks/ConsoleSink.hpp"

#include "RST/log/LogLevel.hpp"
#include "RST/log/LogMessage.hpp"

#include "RST/log/Constants/Terminal.hpp"

#include <chrono>
#include <iomanip>

namespace RST::Log {
    Log::ConsoleSink::ConsoleSink() {
        _colors[LogLevel::Trace] = Terminal::CYAN;
        _colors[LogLevel::Debug] = Terminal::BLUE;
        _colors[LogLevel::Info]  = Terminal::GREEN;
        _colors[LogLevel::Warn]  = Terminal::YELLOW;
        _colors[LogLevel::Error] = Terminal::RED;
        _colors[LogLevel::Fatal] = std::string(Terminal::BOLD) + Terminal::BG_RED + Terminal::WHITE;
    }
    
    void Log::ConsoleSink::setColor(LogLevel level, const std::string& color_code) {
        _colors[level] = color_code;
    }
    
    void Log::ConsoleSink::log(const LogMessage& msg) {
        auto time = std::chrono::system_clock::to_time_t(msg.time);
    
        auto it = _colors.find(msg.level);
        if (it != _colors.end()) {
            std::cout << it->second;
        }
    
        std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] "
                  << "[" << msg.level << "] "
                  << "[" << msg.category << "] "
                  << msg.message << Terminal::RESET << std::endl;
    }
}

