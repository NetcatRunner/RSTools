#include <RST/RST.hpp>

#include <filesystem>
#include <memory>
#include <string>

namespace Sys = RST::System;

static_assert(!Sys::getArchitecture().empty());

TEST_CASE(test_cpu_info) {
    CHECK(Sys::getCpuCores() >= 1u);

    const std::string name = Sys::getCpuName();
    CHECK(RST::String::trimView(name) == name);
#if defined(__linux__) && (defined(__x86_64__) || defined(__i386__))
    CHECK(!name.empty());
#endif
}

// Available RAM counts the reclaimable page cache (MemAvailable), so it is never above the total.
TEST_CASE(test_memory_info) {
    const std::uint64_t total = Sys::getTotalRAM();
    const std::uint64_t available = Sys::getAvailableRAM();
    CHECK(total > 0u);
    CHECK(available > 0u);
    CHECK(available <= total);
}

// Touching 64 MiB shows in the resident set, and the peak is never below the current usage.
TEST_CASE(test_process_memory) {
    const std::uint64_t before = Sys::getProcessMemoryUsage();
    CHECK(before > 0u);

    constexpr std::size_t size = 64u * 1024u * 1024u;
    const auto block = std::make_unique<char[]>(size);
    for (std::size_t i = 0; i < size; i += 4096) {
        block[i] = static_cast<char>(i);
    }
    const std::uint64_t after = Sys::getProcessMemoryUsage();
    CHECK(after >= before + size / 2);
    CHECK(Sys::getPeakProcessMemoryUsage() >= after);
    CHECK(block[4096] == static_cast<char>(4096));
}

TEST_CASE(test_os_and_host) {
    CHECK(!Sys::getOSName().empty());
    CHECK(!Sys::getHostName().empty());
}

TEST_CASE(test_disk_space) {
    const std::uint64_t total = Sys::getTotalDiskSpace();
    CHECK(total > 0u);
    CHECK(Sys::getAvailableDiskSpace() <= total);
    CHECK(Sys::getTotalDiskSpace("/this/path/does/not/exist") == 0u);
}

TEST_CASE(test_environment) {
    const std::string name = "RST_TEST_ENVIRONMENT_VARIABLE";
    Sys::unsetEnv(name);
    CHECK(!Sys::getEnv(name).has_value());

    CHECK(Sys::setEnv(name, "first"));
    CHECK(Sys::getEnv(name) == "first");

    CHECK(Sys::setEnv(name, "second", false));
    CHECK(Sys::getEnv(name) == "first");

    CHECK(Sys::setEnv(name, "second"));
    CHECK(Sys::getEnv(name) == "second");

    CHECK(Sys::unsetEnv(name));
    CHECK(!Sys::getEnv(name).has_value());

    CHECK(!Sys::setEnv("", "value"));
    CHECK(!Sys::setEnv("BAD=NAME", "value"));
    CHECK(!Sys::getEnv("BAD=NAME").has_value());
}

TEST_CASE(test_paths) {
    const std::filesystem::path executable = Sys::getExecutablePath();
    CHECK(executable.is_absolute());
    CHECK(std::filesystem::exists(executable));
    CHECK(executable.stem() == "run_tests");
    CHECK(Sys::getExecutableDirectory() == executable.parent_path());
    CHECK(!Sys::getHomeDirectory().empty());
}

// Binary content comes back byte for byte: embedded NUL and \r\n are kept.
TEST_CASE(test_file_round_trip) {
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "rst_file_test.bin";
    const std::string content("line one\r\nnul:\0:end", 19);

    CHECK(Sys::writeFile(path, content));
    CHECK(Sys::readFile(path) == content);

    CHECK(Sys::appendFile(path, "+tail"));
    CHECK(Sys::readFile(path) == content + "+tail");

    CHECK(Sys::writeFile(path, ""));
    CHECK(Sys::readFile(path) == "");

    std::filesystem::remove(path);
}

TEST_CASE(test_file_errors) {
    CHECK(!Sys::readFile("/this/file/does/not/exist.txt").has_value());
    CHECK(!Sys::readFile(std::filesystem::temp_directory_path()).has_value());
    CHECK(!Sys::writeFile("/this/folder/does/not/exist/file.txt", "x"));
}

#if defined(__linux__)
// /proc files report a size of 0: they are read until the end anyway.
TEST_CASE(test_file_reads_proc_files) {
    const auto status = Sys::readFile("/proc/self/status");
    CHECK(status.has_value());
    CHECK(status->find("VmRSS") != std::string::npos);
}
#endif
