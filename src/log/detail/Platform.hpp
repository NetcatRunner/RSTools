#pragma once

#include <cstdint>
#include <ctime>
#include <iosfwd>
#include <string_view>

namespace RST::Log::detail {

    [[nodiscard]] std::uint64_t currentThreadId() noexcept;
    [[nodiscard]] std::uint64_t currentProcessId() noexcept;

    [[nodiscard]] std::tm localTime(std::time_t time) noexcept;

    [[nodiscard]] bool isColorTerminal(const std::ostream& stream) noexcept;

    [[nodiscard]] bool hasDebuggerOutput() noexcept;
    [[nodiscard]] bool isDebuggerAttached() noexcept;
    void writeToDebugger(const char* text) noexcept;

    void reportInternalError(std::string_view context, std::string_view what) noexcept;
}
