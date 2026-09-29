#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace RST::System {

    /// @name Files
    /// Files are read and written in binary mode, so the content is kept unchanged.
    /// @{

    /// Content of the file, or `std::nullopt` if it cannot be read.
    [[nodiscard]] std::optional<std::string> readFile(const std::filesystem::path& path);

    /// Creates or replaces the file with `content`; returns false on failure.
    bool writeFile(const std::filesystem::path& path, std::string_view content);

    /// Appends `content` to the file, creating it if needed; returns false on failure.
    bool appendFile(const std::filesystem::path& path, std::string_view content);
    /// @}
}
