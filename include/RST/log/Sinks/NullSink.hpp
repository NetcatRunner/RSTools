#pragma once

#include "RST/log/Sinks/ASink.hpp"

namespace RST::Log {

    class NullSink : public ASink {
    protected:
        void log(const LogMessage&) override {}
    };

}
