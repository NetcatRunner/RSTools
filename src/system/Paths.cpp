#include "RST/system/Paths.hpp"

#include <string>
#include <system_error>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#else
#  include "RST/system/Environment.hpp"
#  include <pwd.h>
#  include <unistd.h>
#  include <vector>
#  if defined(__APPLE__)
#    include <cstdint>
#    include <mach-o/dyld.h>
#  endif
#endif

namespace RST::System {

    namespace {

#if defined(_WIN32)
        [[nodiscard]] std::wstring environmentVariable(const wchar_t* name)
        {
            const DWORD size = ::GetEnvironmentVariableW(name, nullptr, 0);
            if (size == 0) {
                return {};
            }
            std::wstring value(size, L'\0');
            const DWORD length = ::GetEnvironmentVariableW(name, value.data(), size);
            if (length == 0 || length >= size) {
                return {};
            }
            value.resize(length);
            return value;
        }
#endif

    }

    std::filesystem::path getExecutablePath()
    {
#if defined(_WIN32)
        std::wstring buffer(MAX_PATH, L'\0');
        while (buffer.size() <= 32768) {
            const DWORD length = ::GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (length == 0) {
                return {};
            }
            if (length < buffer.size()) {
                buffer.resize(length);
                return std::filesystem::path(buffer);
            }
            buffer.resize(buffer.size() * 2);
        }
        return {};
#elif defined(__APPLE__)
        std::uint32_t size = 0;
        ::_NSGetExecutablePath(nullptr, &size);
        std::string buffer(size, '\0');
        if (::_NSGetExecutablePath(buffer.data(), &size) != 0) {
            return {};
        }
        buffer.resize(buffer.find('\0') == std::string::npos ? buffer.size() : buffer.find('\0'));
        std::error_code error;
        std::filesystem::path resolved = std::filesystem::weakly_canonical(buffer, error);
        return error ? std::filesystem::path(buffer) : resolved;
#elif defined(__linux__)
        std::error_code error;
        std::filesystem::path path = std::filesystem::read_symlink("/proc/self/exe", error);
        return error ? std::filesystem::path() : path;
#else
        return {};
#endif
    }

    std::filesystem::path getExecutableDirectory()
    {
        return getExecutablePath().parent_path();
    }

    std::filesystem::path getHomeDirectory()
    {
#if defined(_WIN32)
        if (std::wstring profile = environmentVariable(L"USERPROFILE"); !profile.empty()) {
            return std::filesystem::path(profile);
        }
        const std::wstring drive = environmentVariable(L"HOMEDRIVE");
        const std::wstring home = environmentVariable(L"HOMEPATH");
        return (drive.empty() || home.empty()) ? std::filesystem::path() : std::filesystem::path(drive + home);
#else
        if (const auto home = getEnv("HOME"); home.has_value() && !home->empty()) {
            return std::filesystem::path(*home);
        }
        const long suggested = ::sysconf(_SC_GETPW_R_SIZE_MAX);
        std::vector<char> buffer(suggested > 0 ? static_cast<std::size_t>(suggested) : 16384);
        struct passwd entry{};
        struct passwd* found = nullptr;
        if (::getpwuid_r(::getuid(), &entry, buffer.data(), buffer.size(), &found) != 0 || found == nullptr || found->pw_dir == nullptr) {
            return {};
        }
        return std::filesystem::path(found->pw_dir);
#endif
    }
}
