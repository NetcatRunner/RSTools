#include "RST/log/Sinks/ConsoleSink.hpp"
#include "RST/log/Constants/Terminal.hpp"

namespace RST::Log {

    ConsoleSink::ConsoleSink(std::ostream& target): _target(target), _colorEnabled(Terminal::supportsColor())
    {
        _colors[LogLevel::Trace] = Terminal::CYAN;
        _colors[LogLevel::Debug] = Terminal::BRIGHT_BLUE;
        _colors[LogLevel::Info]  = Terminal::BRIGHT_GREEN;
        _colors[LogLevel::Warn]  = Terminal::BRIGHT_YELLOW;
        _colors[LogLevel::Error] = Terminal::BRIGHT_RED;
        _colors[LogLevel::Fatal] = std::string(Terminal::BOLD) + Terminal::BG_RED + Terminal::WHITE;
    }

    void ConsoleSink::setColor(LogLevel level, std::string_view color_code) {
        _colors[level] = std::string(color_code);
    }

    void ConsoleSink::log(const LogMessage& msg) {
        const std::string line = formatted(msg);

        if (_colorEnabled) {
            auto it = _colors.find(msg.level);
            if (it != _colors.end())
                _target << it->second;
        }

        _target << line;

        if (_colorEnabled)
            _target << Terminal::RESET;

        _target << '\n';
    }

} // namespace RST::Log
