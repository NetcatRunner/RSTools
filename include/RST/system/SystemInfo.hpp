#pragma once

#include <string>
#include <cstdint>
#include <cstddef>

namespace RST::System {
    
    size_t GetCpuCores();

    uint64_t GetTotalRAM();

    uint64_t GetAvailableRAM();

    std::string GetOSName();

    uint64_t GetTotalDiskSpace(const std::string& path = "/");
    uint64_t GetAvailableDiskSpace(const std::string& path = "/");
}