#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>

namespace RST::Log {

    class SessionFileSink : public ASink {
    public:
        static constexpr std::size_t DEFAULT_MAX_BACKUPS = 10;

        explicit SessionFileSink(std::string_view filepath, std::size_t maxBackups = DEFAULT_MAX_BACKUPS);
        ~SessionFileSink() override;

        [[nodiscard]] const std::string& filepath() const noexcept { return _filepath; }

    protected:
        void log(const LogMessage& message) override;
        void flushSink() override;

    private:
        std::string _filepath;
        std::ofstream _file;
    };

}
