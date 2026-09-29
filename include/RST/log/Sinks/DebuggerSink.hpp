#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <string>

namespace RST::Log {

    class DebuggerSink : public ASink {
    public:
        [[nodiscard]] static bool isSupported() noexcept;

    protected:
        void log(const LogMessage& message) override;

    private:
        std::string _text;
    };

}
