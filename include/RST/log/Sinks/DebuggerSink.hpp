#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <string>

namespace RST::Log {

    /// Sends messages to the attached debugger on Windows; does nothing elsewhere.
    class DebuggerSink : public ASink {
    public:
        /// Whether this platform has a debugger output.
        [[nodiscard]] static bool isSupported() noexcept;

    protected:
        void log(const LogMessage& message) override;

    private:
        std::string _text;
    };

}
