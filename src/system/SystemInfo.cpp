#include "RST/system/SystemInfo.hpp"

#include "RST/string/Trim.hpp"

#include <filesystem>
#include <system_error>
#include <thread>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef PSAPI_VERSION
#    define PSAPI_VERSION 2
#  endif
#  include <windows.h>
#  include <psapi.h>
#else
#  include <sys/resource.h>
#  include <unistd.h>
#  if defined(__APPLE__)
#    include <mach/mach.h>
#    include <sys/sysctl.h>
#  elif defined(__linux__)
#    include <sys/sysinfo.h>
#    include <cstdio>
#  endif
#endif

namespace RST::System {

    namespace {

#if !defined(_WIN32)
        [[nodiscard]] std::uint64_t pageSize() noexcept
        {
            const long size = ::sysconf(_SC_PAGESIZE);
            return size > 0 ? static_cast<std::uint64_t>(size) : 4096;
        }
#endif

#if defined(__linux__)
        [[nodiscard]] std::uint64_t meminfoBytes(std::string_view key) noexcept
        {
            std::FILE* file = std::fopen("/proc/meminfo", "r");
            if (file == nullptr) {
                return 0;
            }
            std::uint64_t bytes = 0;
            char line[256];
            while (std::fgets(line, sizeof(line), file) != nullptr) {
                const std::string_view text(line);
                if (text.size() > key.size() && text.starts_with(key) && text[key.size()] == ':') {
                    unsigned long long kibibytes = 0;
                    if (std::sscanf(line + key.size() + 1, "%llu", &kibibytes) == 1) {
                        bytes = kibibytes * 1024;
                    }
                    break;
                }
            }
            std::fclose(file);
            return bytes;
        }
#endif

    }

    std::size_t getCpuCores() noexcept
    {
        const unsigned int count = std::thread::hardware_concurrency();
        return count > 0 ? count : 1;
    }

    std::string getCpuName()
    {
#if defined(_WIN32)
        char name[256] = {};
        DWORD size = sizeof(name);
        if (::RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString",
                           RRF_RT_REG_SZ, nullptr, name, &size) != ERROR_SUCCESS) {
            return {};
        }
        return std::string(String::trimView(name));
#elif defined(__APPLE__)
        char name[256] = {};
        std::size_t size = sizeof(name) - 1;
        if (::sysctlbyname("machdep.cpu.brand_string", name, &size, nullptr, 0) != 0) {
            return {};
        }
        return std::string(String::trimView(name));
#elif defined(__linux__)
        std::FILE* file = std::fopen("/proc/cpuinfo", "r");
        if (file == nullptr) {
            return {};
        }
        std::string modelName;
        std::string hardware;
        char line[512];
        while (modelName.empty() && std::fgets(line, sizeof(line), file) != nullptr) {
            const std::string_view text(line);
            const std::size_t colon = text.find(':');
            if (colon == std::string_view::npos) {
                continue;
            }
            const std::string_view key = String::trimView(text.substr(0, colon));
            if (key == "model name") {
                modelName = String::trimView(text.substr(colon + 1));
            } else if (key == "Hardware") {
                hardware = String::trimView(text.substr(colon + 1));
            }
        }
        std::fclose(file);
        return modelName.empty() ? hardware : modelName;
#else
        return {};
#endif
    }

    std::uint64_t getTotalRAM() noexcept
    {
#if defined(_WIN32)
        MEMORYSTATUSEX status{};
        status.dwLength = sizeof(status);
        return ::GlobalMemoryStatusEx(&status) != 0 ? status.ullTotalPhys : 0;
#elif defined(__APPLE__)
        std::uint64_t size = 0;
        std::size_t length = sizeof(size);
        return ::sysctlbyname("hw.memsize", &size, &length, nullptr, 0) == 0 ? size : 0;
#elif defined(__linux__)
        struct sysinfo info{};
        return ::sysinfo(&info) == 0 ? static_cast<std::uint64_t>(info.totalram) * info.mem_unit : 0;
#else
        return 0;
#endif
    }

    std::uint64_t getAvailableRAM() noexcept
    {
#if defined(_WIN32)
        MEMORYSTATUSEX status{};
        status.dwLength = sizeof(status);
        return ::GlobalMemoryStatusEx(&status) != 0 ? status.ullAvailPhys : 0;
#elif defined(__APPLE__)
        // Free plus inactive pages: what macOS hands out before compressing or swapping.
        vm_statistics64_data_t stats{};
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        const mach_port_t host = ::mach_host_self();
        const kern_return_t result = ::host_statistics64(host, HOST_VM_INFO64, reinterpret_cast<host_info64_t>(&stats), &count);
        ::mach_port_deallocate(::mach_task_self(), host);
        if (result != KERN_SUCCESS) {
            return 0;
        }
        return (static_cast<std::uint64_t>(stats.free_count) + stats.inactive_count) * pageSize();
#elif defined(__linux__)
        if (const std::uint64_t available = meminfoBytes("MemAvailable"); available > 0) {
            return available;
        }
        struct sysinfo info{};
        return ::sysinfo(&info) == 0 ? (static_cast<std::uint64_t>(info.freeram) + info.bufferram) * info.mem_unit : 0;
#else
        return 0;
#endif
    }

    std::uint64_t getProcessMemoryUsage() noexcept
    {
#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS counters{};
        return ::GetProcessMemoryInfo(::GetCurrentProcess(), &counters, static_cast<DWORD>(sizeof(counters))) != 0 ? counters.WorkingSetSize : 0;
#elif defined(__APPLE__)
        mach_task_basic_info_data_t info{};
        mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
        if (::task_info(::mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &count) != KERN_SUCCESS) {
            return 0;
        }
        return info.resident_size;
#elif defined(__linux__)
        std::FILE* file = std::fopen("/proc/self/statm", "r");
        if (file == nullptr) {
            return 0;
        }
        unsigned long long totalPages = 0;
        unsigned long long residentPages = 0;
        const int fields = std::fscanf(file, "%llu %llu", &totalPages, &residentPages);
        std::fclose(file);
        return fields == 2 ? residentPages * pageSize() : 0;
#else
        return 0;
#endif
    }

    std::uint64_t getPeakProcessMemoryUsage() noexcept
    {
        std::uint64_t peak = 0;
#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS counters{};
        if (::GetProcessMemoryInfo(::GetCurrentProcess(), &counters, static_cast<DWORD>(sizeof(counters))) != 0) {
            peak = counters.PeakWorkingSetSize;
        }
#else
        struct rusage usage{};
        if (::getrusage(RUSAGE_SELF, &usage) == 0) {
#  if defined(__APPLE__)
            peak = static_cast<std::uint64_t>(usage.ru_maxrss);  // bytes on macOS
#  else
            peak = static_cast<std::uint64_t>(usage.ru_maxrss) * 1024;  // kilobytes on Linux and the BSDs
#  endif
        }
#endif
        const std::uint64_t current = getProcessMemoryUsage();
        return peak > current ? peak : current;
    }

    std::string getOSName()
    {
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

    std::string getHostName()
    {
#if defined(_WIN32)
        char name[256] = {};
        DWORD size = sizeof(name);
        if (::GetComputerNameExA(ComputerNameDnsHostname, name, &size) == 0) {
            return {};
        }
        return std::string(name, size);
#else
        char name[256] = {};
        if (::gethostname(name, sizeof(name) - 1) != 0) {
            return {};
        }
        return name;
#endif
    }

    std::uint64_t getTotalDiskSpace(const std::string& path)
    {
        std::error_code error;
        const std::filesystem::space_info info = std::filesystem::space(path, error);
        return error ? 0 : info.capacity;
    }

    std::uint64_t getAvailableDiskSpace(const std::string& path)
    {
        std::error_code error;
        const std::filesystem::space_info info = std::filesystem::space(path, error);
        return error ? 0 : info.available;
    }
}
