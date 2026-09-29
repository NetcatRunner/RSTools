#pragma once

#include <filesystem>

namespace RST::System {

    [[nodiscard]] std::filesystem::path getExecutablePath();

    [[nodiscard]] std::filesystem::path getExecutableDirectory();

    [[nodiscard]] std::filesystem::path getHomeDirectory();
}
