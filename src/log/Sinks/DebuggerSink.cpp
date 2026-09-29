#include "RST/log/Sinks/DebuggerSink.hpp"

#include "log/detail/Platform.hpp"

namespace RST::Log {

    bool DebuggerSink::isSupported() noexcept
    {
        return detail::hasDebuggerOutput();
    }

    void DebuggerSink::log(const LogMessage& message)
    {
        if (!detail::isDebuggerAttached()) {
            return;
        }
        const std::string_view line = formatted(message);
        _text.assign(line);
        _text += '\n';
        detail::writeToDebugger(_text.c_str());
    }

}
