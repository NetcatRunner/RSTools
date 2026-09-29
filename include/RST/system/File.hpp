#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace RST::System {

    [[nodiscard]] std::optional<std::string> readFile(const std::filesystem::path& path);

    bool writeFile(const std::filesystem::path& path, std::string_view content);

    bool appendFile(const std::filesystem::path& path, std::string_view content);
}
