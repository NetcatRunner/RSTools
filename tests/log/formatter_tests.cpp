#include <RST/RST.hpp>

TEST_CASE(test_default_pattern) {
    RST::Log::Formatter fmt;
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "core", "hello world");
    std::string out = fmt.format(msg);
    CHECK(out.find("INFO") != std::string::npos);
    CHECK(out.find("core") != std::string::npos);
    CHECK(out.find("hello world") != std::string::npos);
}

TEST_CASE(test_level_tokens) {
    RST::Log::Formatter fmt("%l | %L");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Warn, "x", "y");
    std::string out = fmt.format(msg);
    CHECK(out.find("WARN") != std::string::npos);
    CHECK(out.find("WRN") != std::string::npos);
}

TEST_CASE(test_logger_name_token) {
    RST::Log::Formatter fmt("%n");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "MyModule", "...");
    CHECK(fmt.format(msg) == "MyModule");
}

TEST_CASE(test_message_token) {
    RST::Log::Formatter fmt("%v");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "payload");
    CHECK(fmt.format(msg) == "payload");
}

TEST_CASE(test_literal_percent) {
    RST::Log::Formatter fmt("100%% done");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "");
    CHECK(fmt.format(msg) == "100% done");
}

TEST_CASE(test_source_location) {
    RST::Log::Formatter fmt("%f:%#");
    RST::Log::SourceLocation src{"src/main.cpp", "myFunc", 42};
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "msg", src);
    std::string out = fmt.format(msg);
    CHECK(out.find("main.cpp") != std::string::npos);
    CHECK(out.find("42") != std::string::npos);
}

TEST_CASE(test_no_source_location) {
    RST::Log::Formatter fmt("%f:%#");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "msg");
    std::string out = fmt.format(msg);
    CHECK(out == ":");
}

TEST_CASE(test_setPattern) {
    RST::Log::Formatter fmt;
    fmt.setPattern("%v");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "simple");
    CHECK(fmt.format(msg) == "simple");
}

TEST_CASE(test_milliseconds_padded) {
    RST::Log::Formatter fmt("%e");
    RST::Log::LogMessage msg(RST::Log::LogLevel::Info, "x", "");
    std::string ms = fmt.format(msg);
    CHECK(ms.size() == 3);
}
