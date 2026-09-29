#include <RST/RST.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace {

    namespace fs = std::filesystem;

    // Each test works in a directory of its own, removed on the way in and on the way out.
    class TempDirectory {
    public:
        explicit TempDirectory(const std::string& name) : _path(fs::temp_directory_path() / ("rst_log_" + name))
        {
            fs::remove_all(_path);
            fs::create_directories(_path);
        }
        ~TempDirectory() { fs::remove_all(_path); }

        [[nodiscard]] const fs::path& path() const { return _path; }

    private:
        fs::path _path;
    };

    [[nodiscard]] std::string readAll(const fs::path& file)
    {
        std::ifstream in(file, std::ios::binary);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    }

    [[nodiscard]] std::vector<fs::path> backupsIn(const fs::path& directory)
    {
        std::vector<fs::path> found;
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.path().filename().string().find("-backup-") != std::string::npos) {
                found.push_back(entry.path());
            }
        }
        std::ranges::sort(found);
        return found;
    }

    void writeSession(const fs::path& file, std::size_t maxBackups, const std::string& text)
    {
        RST::Log::SessionFileSink sink(file.string(), maxBackups);
        sink.setPattern("%v");
        sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "session", text));
        sink.flush();
    }

}

// Each session starts on an empty file; the previous one is kept, whole, beside it.
TEST_CASE(test_session_sink_keeps_the_previous_session) {
    const TempDirectory directory("session_backup");
    const fs::path file = directory.path() / "Game.log";

    writeSession(file, 5, "first session");
    writeSession(file, 5, "second session");

    CHECK(readAll(file) == "second session\n");
    const std::vector<fs::path> backups = backupsIn(directory.path());
    CHECK(backups.size() == 1U);
    if (backups.size() == 1U) {
        CHECK(readAll(backups[0]) == "first session\n");
        CHECK(backups[0].filename().string().starts_with("Game-backup-"));
        CHECK(backups[0].extension() == ".log");
    }
}

// Only the most recent backups stay, however many sessions ran.
TEST_CASE(test_session_sink_prunes_old_backups) {
    const TempDirectory directory("session_prune");
    const fs::path file = directory.path() / "Game.log";

    for (int session = 0; session < 6; ++session) {
        writeSession(file, 2, "session " + std::to_string(session));
    }

    const std::vector<fs::path> backups = backupsIn(directory.path());
    CHECK(backups.size() == 2U);
    CHECK(readAll(file) == "session 5\n");

    // The two kept are the two most recent: sessions 3 and 4.
    std::vector<std::string> kept;
    for (const fs::path& backup : backups) {
        kept.push_back(readAll(backup));
    }
    std::ranges::sort(kept);
    CHECK(kept.size() == 2U);
    if (kept.size() == 2U) {
        CHECK(kept[0] == "session 3\n");
        CHECK(kept[1] == "session 4\n");
    }
}

// maxBackups = 0: the previous session is simply dropped.
TEST_CASE(test_session_sink_without_backups) {
    const TempDirectory directory("session_none");
    const fs::path file = directory.path() / "Game.log";

    writeSession(file, 0, "first");
    writeSession(file, 0, "second");

    CHECK(readAll(file) == "second\n");
    CHECK(backupsIn(directory.path()).empty());
}

// The directory of the log is created if missing: user://logs/ may not exist yet.
TEST_CASE(test_session_sink_creates_its_directory) {
    const TempDirectory directory("session_mkdir");
    const fs::path file = directory.path() / "nested" / "logs" / "Game.log";

    writeSession(file, 3, "hello");

    CHECK(readAll(file) == "hello\n");
}

// maxFiles = 0 used to underflow into 2^64 renames; it now means "no backup".
TEST_CASE(test_rotating_sink_without_backups_terminates) {
    const TempDirectory directory("rotating_zero");
    const fs::path file = directory.path() / "app.log";

    RST::Log::RotatingFileSink sink(file.string(), 64, 0);
    sink.setPattern("%v");
    for (int i = 0; i < 50; ++i) {
        sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "rot", "a line of about thirty chars"));
    }
    sink.flush();

    CHECK(fs::file_size(file) <= 64U);
    CHECK(!fs::exists(directory.path() / "app.log.1.log"));
}

TEST_CASE(test_rotating_sink_keeps_its_backups) {
    const TempDirectory directory("rotating_keep");
    const fs::path file = directory.path() / "app.log";

    RST::Log::RotatingFileSink sink(file.string(), 100, 2);
    sink.setPattern("%v");
    for (int i = 0; i < 40; ++i) {
        sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "rot", "message number " + std::to_string(i)));
    }
    sink.flush();

    CHECK(fs::exists(directory.path() / "app.log.1.log"));
    CHECK(fs::exists(directory.path() / "app.log.2.log"));
    CHECK(!fs::exists(directory.path() / "app.log.3.log"));
}
