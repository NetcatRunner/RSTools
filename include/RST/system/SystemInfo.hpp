#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace RST::System {

    /// @name CPU
    /// @{

    /// Number of logical cores, at least 1.
    [[nodiscard]] std::size_t getCpuCores() noexcept;
    /// CPU model name, or an empty string if unknown.
    [[nodiscard]] std::string getCpuName();
    /// Architecture the program was built for: `x86_64`, `x86`, `arm64`, `arm`, `riscv`, `wasm` or `unknown`.
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
    /// @}

    /// @name Memory
    /// Sizes in bytes, or 0 if unavailable.
    /// @{
    [[nodiscard]] std::uint64_t getTotalRAM() noexcept;
    [[nodiscard]] std::uint64_t getAvailableRAM() noexcept;

    /// Physical memory used by this process.
    [[nodiscard]] std::uint64_t getProcessMemoryUsage() noexcept;
    /// Highest physical memory used by this process so far.
    [[nodiscard]] std::uint64_t getPeakProcessMemoryUsage() noexcept;
    /// @}

    /// @name Operating system
    /// @{

    /// Operating system family, such as `Linux` or `Windows 64-bit`.
    [[nodiscard]] std::string getOSName();

    /// Network name of this computer, or an empty string on failure.
    [[nodiscard]] std::string getHostName();
    /// @}

    /// @name Disk
    /// Sizes in bytes of the file system holding `path`, or 0 on failure.
    /// @{
    [[nodiscard]] std::uint64_t getTotalDiskSpace(const std::string& path = "/");
    [[nodiscard]] std::uint64_t getAvailableDiskSpace(const std::string& path = "/");
    /// @}
}
