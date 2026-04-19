#include <RST/RST.hpp>

#include <cstdio>
#include <string>

static std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f.is_open())
        return ""; 
    return std::string(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
}

TEST_CASE(test_file_sink_writes) {
    const std::string path = "/tmp/rst_test_filesink.log";
    std::remove(path.c_str());

    RST::Log::FileSink sink(path, true);
    sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "test", "line one"));
    sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Warn, "test", "line two"));
    sink.flush();
    
    std::string content = readFile(path);
    CHECK(content.find("line one") != std::string::npos);
    CHECK(content.find("line two") != std::string::npos);
    std::remove(path.c_str());
}

TEST_CASE(test_file_sink_append) {
    const std::string path = "/tmp/rst_test_append.log";
    std::remove(path.c_str());

    RST::Log::FileSink s1(path, false);
    s1.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "a", "first"));
    RST::Log::FileSink s2(path, false);
    s2.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "a", "second"));
    s1.flush();
    s2.flush();

    std::string content = readFile(path);
    CHECK(content.find("first")  != std::string::npos);
    CHECK(content.find("second") != std::string::npos);
    std::remove(path.c_str());
}

TEST_CASE(test_rotating_sink_rotates) {
    const std::string path = "/tmp/rst_rotating_test.log";
    std::remove(path.c_str());
    std::remove((path + ".1.log").c_str());

    RST::Log::RotatingFileSink sink(path, 200, 3);
    for (int i = 0; i < 20; ++i)
        sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "rot", "message de test numéro " + std::to_string(i)));
    sink.flush();

    std::ifstream f(path);
    CHECK(f.good());
    std::remove(path.c_str());
    std::remove((path + ".1.log").c_str());
    std::remove((path + ".2.log").c_str());
    std::remove((path + ".3.log").c_str());
}

TEST_CASE(test_console_sink_no_crash) {
    std::ostringstream oss;
    RST::Log::ConsoleSink sink(oss);
    sink.setColorEnabled(false);
    sink.write(RST::Log::LogMessage(RST::Log::LogLevel::Info, "con", "test output"));
    CHECK(!oss.str().empty());
    CHECK(oss.str().find("test output") != std::string::npos);
}
