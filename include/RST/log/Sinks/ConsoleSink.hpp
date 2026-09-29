#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <array>
#include <atomic>
#include <iostream>
#include <string>
#include <string_view>

namespace RST::Log {

    /// Writes colored lines to `std::cout` or another stream.
    ///
    /// Colors are enabled when the stream is a terminal that supports them.
    class ConsoleSink : public ASink {
    public:
        explicit ConsoleSink(std::ostream& target = std::cout);

        /// Sets the ANSI color of `level`; see Terminal.
        void setColor(LogLevel level, std::string_view colorCode);
        void setColorEnabled(bool enabled) noexcept { _colorEnabled.store(enabled, std::memory_order_relaxed); }
        [[nodiscard]] bool isColorEnabled() const noexcept { return _colorEnabled.load(std::memory_order_relaxed); }

    protected:
        void log(const LogMessage& message) override;
        void flushSink() override;

    private:
        std::ostream& _target;
        std::array<std::string, kLogLevelCount> _colors;
        std::atomic<bool> _colorEnabled;
    };

}
