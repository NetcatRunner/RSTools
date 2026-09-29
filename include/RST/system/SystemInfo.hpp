#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace RST::System {

    // ── CPU ──────────────────────────────────────────────────────────────────
    [[nodiscard]] std::size_t getCpuCores() noexcept;
    [[nodiscard]] std::string getCpuName();
    [[nodiscard]] constexpr std::string_view getArchitecture() noexcept
    {
#if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
        return "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
        return "arm64";
#elif defined(__arm__) || defined(_M_ARM)
        return "arm";
#elif defined(__riscv)
        return "riscv";
#elif defined(__wasm__)
        return "wasm";
#else
        return "unknown";
#endif
    }

    // ── Memory ────────────────────────────────────
    [[nodiscard]] std::uint64_t getTotalRAM() noexcept;
    [[nodiscard]] std::uint64_t getAvailableRAM() noexcept;

    [[nodiscard]] std::uint64_t getProcessMemoryUsage() noexcept;
    [[nodiscard]] std::uint64_t getPeakProcessMemoryUsage() noexcept;

    // ── Operating system ─────────────────────────────────────────────────────
    [[nodiscard]] std::string getOSName();

    [[nodiscard]] std::string getHostName();

    // ── Disk ──────────────────
    [[nodiscard]] std::uint64_t getTotalDiskSpace(const std::string& path = "/");
    [[nodiscard]] std::uint64_t getAvailableDiskSpace(const std::string& path = "/");
}
