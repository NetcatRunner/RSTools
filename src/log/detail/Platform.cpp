#include "log/detail/Platform.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <io.h>
#  include <process.h>
#  ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#else
#  include <unistd.h>
#  if defined(__linux__)
#    include <sys/syscall.h>
#  elif defined(__APPLE__)
#    include <pthread.h>
#  else
#    include <functional>
#    include <thread>
#  endif
#endif

namespace RST::Log::detail {

    namespace {

        [[nodiscard]] std::uint64_t queryThreadId() noexcept
        {
#if defined(_WIN32)
            return static_cast<std::uint64_t>(::GetCurrentThreadId());
#elif defined(__linux__)
            return static_cast<std::uint64_t>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
            std::uint64_t id = 0;
            ::pthread_threadid_np(nullptr, &id);
            return id;
#else
            return static_cast<std::uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
#endif
        }

        [[nodiscard]] int descriptorOf(const std::ostream& stream) noexcept
        {
            if (&stream == &std::cout) {
                return 1;
            }
            if (&stream == &std::cerr || &stream == &std::clog) {
                return 2;
            }
            return -1;
        }

    }

    std::uint64_t currentThreadId() noexcept
    {
        thread_local const std::uint64_t id = queryThreadId();
        return id;
    }

    std::uint64_t currentProcessId() noexcept
    {
#if defined(_WIN32)
        return static_cast<std::uint64_t>(::_getpid());
#else
        return static_cast<std::uint64_t>(::getpid());
#endif
    }

    std::tm localTime(std::time_t time) noexcept
    {
        std::tm result{};
#if defined(_WIN32)
        ::localtime_s(&result, &time);
#else
        ::localtime_r(&time, &result);
#endif
        return result;
    }

    bool isColorTerminal(const std::ostream& stream) noexcept
    {
        const int descriptor = descriptorOf(stream);
        if (descriptor < 0) {
            return false;
        }
#if defined(_WIN32)
        if (::_isatty(descriptor) == 0) {
            return false;
        }
        const HANDLE handle = ::GetStdHandle(descriptor == 1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        DWORD mode = 0;
        if (handle == INVALID_HANDLE_VALUE || ::GetConsoleMode(handle, &mode) == 0) {
            return false;
        }
        return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0
            || ::SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        if (::isatty(descriptor) == 0) {
            return false;
        }
        const char* term = std::getenv("TERM");
        return term != nullptr && std::string_view(term) != "dumb";
#endif
    }

    bool hasDebuggerOutput() noexcept
    {
#if defined(_WIN32)
        return true;
#else
        return false;
#endif
    }

    bool isDebuggerAttached() noexcept
    {
#if defined(_WIN32)
        return ::IsDebuggerPresent() != 0;
#else
        return false;
#endif
    }

    void writeToDebugger([[maybe_unused]] const char* text) noexcept
    {
#if defined(_WIN32)
        ::OutputDebugStringA(text);
#endif
    }

    void reportInternalError(std::string_view context, std::string_view what) noexcept
    {
        std::fprintf(stderr, "[RST::Log] %.*s: %.*s\n",
                     static_cast<int>(context.size()), context.data(),
                     static_cast<int>(what.size()), what.data());
        std::fflush(stderr);
    }

}
