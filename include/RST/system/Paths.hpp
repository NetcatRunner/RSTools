#pragma once

#include <filesystem>

namespace RST::System {

    /// @name Paths
    /// Empty when the path cannot be found.
    /// @{

    /// Absolute path of the running executable.
    [[nodiscard]] std::filesystem::path getExecutablePath();

    /// Directory of the running executable.
    [[nodiscard]] std::filesystem::path getExecutableDirectory();

    /// Home directory of the current user.
    [[nodiscard]] std::filesystem::path getHomeDirectory();
    /// @}
}
