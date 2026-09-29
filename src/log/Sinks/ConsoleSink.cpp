#include "RST/log/Sinks/ConsoleSink.hpp"

#include "RST/log/Constants/Terminal.hpp"

#include "log/detail/Platform.hpp"

#include <mutex>

namespace RST::Log {

    namespace {

        [[nodiscard]] std::size_t indexOf(LogLevel level) noexcept
        {
            return static_cast<std::size_t>(level);
        }

    }

    ConsoleSink::ConsoleSink(std::ostream& target)
        : _target(target), _colorEnabled(detail::isColorTerminal(target))
    {
        _colors[indexOf(LogLevel::Trace)] = Terminal::CYAN;
        _colors[indexOf(LogLevel::Debug)] = Terminal::BRIGHT_BLUE;
        _colors[indexOf(LogLevel::Info)]  = Terminal::BRIGHT_GREEN;
        _colors[indexOf(LogLevel::Warn)]  = Terminal::BRIGHT_YELLOW;
        _colors[indexOf(LogLevel::Error)] = Terminal::BRIGHT_RED;
        _colors[indexOf(LogLevel::Fatal)] = std::string(Terminal::BOLD) + Terminal::BG_RED + Terminal::WHITE;
    }

    void ConsoleSink::setColor(LogLevel level, std::string_view colorCode)
    {
        std::lock_guard lock(sinkMutex());
        _colors[indexOf(level)] = std::string(colorCode);
    }

    void ConsoleSink::log(const LogMessage& message)
    {
        const std::string_view line = formatted(message);
        const std::string& color = _colors[indexOf(message.level)];
        const bool colored = isColorEnabled() && !color.empty();

        if (colored) {
            _target << color;
        }
        _target.write(line.data(), static_cast<std::streamsize>(line.size()));
        if (colored) {
            _target << Terminal::RESET;
        }
        _target.put('\n');
        _target.flush();
    }

    void ConsoleSink::flushSink()
    {
        _target.flush();
    }

}
