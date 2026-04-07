#pragma once

#include "Sink.hpp"

namespace RST::Log {

    class LogMessage;

    class ASink : public ISink {
    protected:
        std::string _pattern;
    public:
        virtual ~ASink() = default;

        void set_pattern(const std::string &pattern) override {_pattern = pattern;};
    };
}