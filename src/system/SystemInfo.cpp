#include "RST/system/SystemInfo.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/sysinfo.h>
#endif

#include <filesystem>
#include <thread>

namespace RST::System {
    
    size_t GetCpuCores() {
        return std::thread::hardware_concurrency();
    }

    uint64_t GetTotalRAM() {
    #ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys;
    #elif __linux__
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            return static_cast<uint64_t>(info.totalram) * info.mem_unit;
        }
        return 0;
    #else
        return 0;
    #endif
    }

    uint64_t GetAvailableRAM() {
    #ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullAvailPhys;
    #elif __linux__
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            return static_cast<uint64_t>(info.freeram) * info.mem_unit;
        }
        return 0;
    #else
        return 0;
    #endif
    }

    std::string GetOSName() {
    #ifdef _WIN64
        return "Windows 64-bit";
    #elif _WIN32
        return "Windows 32-bit";
    #elif __APPLE__ || __MACH__
        return "Mac OSX";
    #elif __linux__
        return "Linux";
    #elif __FreeBSD__
        return "FreeBSD";
    #elif __unix || __unix__
        return "Unix";
    #else
        return "Other";
    #endif
    }

    uint64_t GetTotalDiskSpace(const std::string& path)
    {
        try {
            std::filesystem::space_info info = std::filesystem::space(path);
            return info.capacity;
        } catch (...) {
            return 0;
        }
    }

    uint64_t GetAvailableDiskSpace(const std::string& path)
    {
        try {
            std::filesystem::space_info info = std::filesystem::space(path);
            return info.available;
        } catch (...) {
            return 0;
        }
    }
}