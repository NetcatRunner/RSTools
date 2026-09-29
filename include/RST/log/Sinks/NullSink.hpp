#pragma once

#include "RST/log/Sinks/ASink.hpp"

namespace RST::Log {

    /// Discards every message.
    class NullSink : public ASink {
    protected:
        void log(const LogMessage&) override {}
    };

}
