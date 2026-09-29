#include "RST/system/File.hpp"

#include <fstream>
#include <system_error>

namespace RST::System {

    namespace {

        [[nodiscard]] bool writeTo(const std::filesystem::path& path, std::string_view content, std::ios::openmode mode)
        {
            std::ofstream file(path, std::ios::binary | mode);
            if (!file) {
                return false;
            }
            file.write(content.data(), static_cast<std::streamsize>(content.size()));
            file.close();
            return !file.fail();
        }

    }

    std::optional<std::string> readFile(const std::filesystem::path& path)
    {
        std::error_code error;
        if (std::filesystem::is_directory(path, error)) {
            return std::nullopt;
        }
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return std::nullopt;
        }

        std::string content;
        const std::uintmax_t size = std::filesystem::file_size(path, error);
        if (!error && size > 0) {
            content.resize(static_cast<std::size_t>(size));
            file.read(content.data(), static_cast<std::streamsize>(content.size()));
            content.resize(static_cast<std::size_t>(file.gcount()));
        }
        char buffer[16 * 1024];
        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            content.append(buffer, static_cast<std::size_t>(file.gcount()));
        }
        if (file.bad()) {
            return std::nullopt;
        }
        return content;
    }

    bool writeFile(const std::filesystem::path& path, std::string_view content)
    {
        return writeTo(path, content, std::ios::trunc);
    }

    bool appendFile(const std::filesystem::path& path, std::string_view content)
    {
        return writeTo(path, content, std::ios::app);
    }
}
