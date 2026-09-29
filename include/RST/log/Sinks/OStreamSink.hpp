#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <ostream>

namespace RST::Log {

    /// Writes plain lines to a `std::ostream`, which must outlive the sink.
    class OStreamSink : public ASink {
    public:
        explicit OStreamSink(std::ostream& stream) : _stream(stream) {}

    protected:
        void log(const LogMessage& message) override
        {
            const std::string_view line = formatted(message);
            _stream.write(line.data(), static_cast<std::streamsize>(line.size()));
            _stream.put('\n');
        }

        void flushSink() override { _stream.flush(); }

    private:
        std::ostream& _stream;
    };

}
