#include <RST/system/System.hpp>
#include <RST/time/Time.hpp>

#include <filesystem>
#include <iostream>

int main() {
    namespace Sys = RST::System;

    const RST::Time::Timer timer = RST::Time::Timer::startNew();

    // Machine
    std::cout << Sys::getOSName() << " (" << Sys::getArchitecture() << "), " << Sys::getCpuName() << '\n';
    std::cout << Sys::getCpuCores() << " cores, " << Sys::getAvailableRAM() / (1024 * 1024) << " MiB of RAM available\n";

    // Environment and paths
    std::cout << "home: " << Sys::getHomeDirectory().string() << '\n';
    std::cout << "shell: " << Sys::getEnv("SHELL").value_or("unknown") << '\n';

    // Files, without exceptions
    const std::filesystem::path note = std::filesystem::temp_directory_path() / "rstools-note.txt";
    if (Sys::writeFile(note, "written at " + RST::Time::formatNow()))
        std::cout << Sys::readFile(note).value_or("") << '\n';

    std::cout << "done in " << timer.getElapsedMs() << " ms\n";
    return 0;
}
