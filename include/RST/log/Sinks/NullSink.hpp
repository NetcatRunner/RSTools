#pragma once
#include "ASink.hpp"

namespace RST::Log {

class NullSink : public ASink {
protected:
    void log(const LogMessage&) override {}
};

} // namespace RST::Log
