#include "RST/log/Sinks/SessionFileSink.hpp"

#include "log/detail/Platform.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <ios>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace RST::Log {

    namespace {
        constexpr std::string_view kBackupMarker = "-backup-";

        [[nodiscard]] std::string lastWriteStamp(const std::filesystem::path& file)
        {
            std::error_code error;
            const std::filesystem::file_time_type written = std::filesystem::last_write_time(file, error);
            const auto when = error ? std::chrono::system_clock::now() : std::chrono::clock_cast<std::chrono::system_clock>(written);
            const std::tm calendar = detail::localTime(std::chrono::system_clock::to_time_t(when));
            return std::format("{:04}.{:02}.{:02}-{:02}.{:02}.{:02}",
                               calendar.tm_year + 1900, calendar.tm_mon + 1, calendar.tm_mday,
                               calendar.tm_hour, calendar.tm_min, calendar.tm_sec);
        }

        [[nodiscard]] std::filesystem::path backupPathFor(const std::filesystem::path& file, const std::string& stamp)
        {
            const std::string stem = file.stem().string();
            const std::string extension = file.extension().string();
            std::filesystem::path candidate = file.parent_path() / (stem + std::string(kBackupMarker) + stamp + extension);

            std::error_code error;
            for (int suffix = 1; std::filesystem::exists(candidate, error) && suffix < 1000; ++suffix) {
                candidate = file.parent_path() / std::format("{}{}{}-{}{}", stem, kBackupMarker, stamp, suffix, extension);
            }
            return candidate;
        }

        void pruneBackups(const std::filesystem::path& file, std::size_t maxBackups)
        {
            const std::string prefix = file.stem().string() + std::string(kBackupMarker);
            const std::string extension = file.extension().string();
            const std::filesystem::path directory = file.parent_path().empty() ? std::filesystem::path(".") : file.parent_path();

            struct Backup {
                std::filesystem::file_time_type written;
                std::filesystem::path path;
            };

            std::vector<Backup> backups;
            std::error_code error;
            for (std::filesystem::directory_iterator it(directory, error), end; !error && it != end; it.increment(error)) {
                const std::string name = it->path().filename().string();
                if (name.starts_with(prefix) && name.ends_with(extension)) {
                    std::error_code timeError;
                    backups.push_back({std::filesystem::last_write_time(it->path(), timeError), it->path()});
                }
            }
            if (backups.size() <= maxBackups) {
                return;
            }

            std::ranges::sort(backups, [](const Backup& a, const Backup& b) {
                return a.written != b.written ? a.written < b.written : a.path < b.path;
            });
            const auto excess = static_cast<std::ptrdiff_t>(backups.size() - maxBackups);
            for (auto it = backups.begin(); it != backups.begin() + excess; ++it) {
                std::filesystem::remove(it->path, error);
            }
        }

        void archivePreviousSession(const std::filesystem::path& file, std::size_t maxBackups) noexcept
        {
            try {
                std::error_code error;
                if (!file.parent_path().empty()) {
                    std::filesystem::create_directories(file.parent_path(), error);
                }
                if (std::filesystem::exists(file, error) && std::filesystem::file_size(file, error) > 0 && !error) {
                    if (maxBackups == 0) {
                        std::filesystem::remove(file, error);
                    } else {
                        std::filesystem::rename(file, backupPathFor(file, lastWriteStamp(file)), error);
                    }
                }
                pruneBackups(file, maxBackups);
            } catch (const std::exception& failure) {
                detail::reportInternalError("SessionFileSink: previous session not archived", failure.what());
            }
        }

    }

    SessionFileSink::SessionFileSink(std::string_view filepath, std::size_t maxBackups)
        : _filepath(filepath)
    {
        archivePreviousSession(std::filesystem::path(_filepath), maxBackups);

        _file.open(_filepath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!_file.is_open()) {
            throw std::runtime_error("SessionFileSink: cannot open " + _filepath);
        }
    }

    SessionFileSink::~SessionFileSink()
    {
        if (_file.is_open()) {
            _file.flush();
        }
    }

    void SessionFileSink::log(const LogMessage& message)
    {
        const std::string_view line = formatted(message);
        _file.write(line.data(), static_cast<std::streamsize>(line.size()));
        _file.put('\n');
    }

    void SessionFileSink::flushSink()
    {
        _file.flush();
    }

}
