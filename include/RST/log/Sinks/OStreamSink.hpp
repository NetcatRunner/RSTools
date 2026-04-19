#pragma once
#include "ASink.hpp"
#include <ostream>

namespace RST::Log {

class OStreamSink : public ASink {
public:
    OStreamSink(std::ostream& stream) : _stream(stream) {}

    void flush() override { _stream.flush(); }

protected:
    void log(const LogMessage& msg) override {
        _stream << formatted(msg) << '\n';
    }

private:
    std::ostream& _stream;
};

} // namespace RST::Log
