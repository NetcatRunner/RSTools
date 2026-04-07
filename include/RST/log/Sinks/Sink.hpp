#pragma once

#include <string>
#include <memory>

namespace RST::Log {

    class LogMessage;

    class ISink {
    public:
        virtual ~ISink() = default;

        virtual void log(const LogMessage& msg) = 0;
        virtual void set_pattern(const std::string &pattern) = 0;
    };

    using sink_ptr = std::shared_ptr<ISink>;
}
