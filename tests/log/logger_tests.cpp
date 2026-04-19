#include <RST/RST.hpp>

class TestSink : public RST::Log::ASink {
public:
    std::vector<RST::Log::LogMessage> messages;
protected:
    void log(const RST::Log::LogMessage& msg) override {
        messages.push_back(msg);
    }
};

TEST_CASE(test_logger_name) {
    RST::Log::Logger logger("TestLogger");
    CHECK(logger.getName() == "TestLogger");
}

TEST_CASE(test_level_filtering) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("FilterTest", RST::Log::LogLevel::Warn);
    logger.addSink(sink);

    logger.debug("should be filtered");
    logger.info("should be filtered");
    logger.warn("should pass");
    logger.error("should pass");

    CHECK(sink->messages.size() == 2);
    CHECK(sink->messages[0].level == RST::Log::LogLevel::Warn);
    CHECK(sink->messages[1].level == RST::Log::LogLevel::Error);
}

TEST_CASE(test_set_level) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("LevelTest", RST::Log::LogLevel::Error);
    logger.addSink(sink);

    logger.info("filtered");
    CHECK(sink->messages.empty());

    logger.setLevel(RST::Log::LogLevel::Info);
    logger.info("passes now");
    CHECK(sink->messages.size() == 1);
}

TEST_CASE(test_message_content) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("MsgTest");
    logger.addSink(sink);

    logger.info("Hello {}", "World");
    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0].message == "Hello World");
    CHECK(sink->messages[0].category == "MsgTest");
    CHECK(sink->messages[0].level == RST::Log::LogLevel::Info);
}

TEST_CASE(test_format_multiple_args) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("FmtTest");
    logger.addSink(sink);

    logger.debug("val={} str={} num={}", 42, "abc", 3.14f);
    CHECK(!sink->messages.empty());
    CHECK(sink->messages[0].message.find("{}") == std::string::npos);
    CHECK(sink->messages[0].message.find("42") != std::string::npos);
    CHECK(sink->messages[0].message.find("abc") != std::string::npos);
}

TEST_CASE(test_source_location_captured) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("SrcTest");
    logger.addSink(sink);

    RST::Log::SourceLocation src{"file.cpp", "myFunc", 99};
    logger.log(RST::Log::LogLevel::Info, src, "with source");

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0].source->line == 99);
}

TEST_CASE(test_multiple_sinks) {
    auto s1 = std::make_shared<TestSink>();
    auto s2 = std::make_shared<TestSink>();
    RST::Log::Logger logger("MultiSink");
    logger.addSink(s1);
    logger.addSink(s2);

    logger.info("broadcast");

    CHECK(s1->messages.size() == 1);
    CHECK(s2->messages.size() == 1);
}

TEST_CASE(test_clear_sinks) {
    auto sink = std::make_shared<TestSink>();
    RST::Log::Logger logger("ClearTest");
    logger.addSink(sink);
    CHECK(logger.sinkCount() == 1);

    logger.clearSinks();
    CHECK(logger.sinkCount() == 0);

    logger.info("should not arrive");
    CHECK(sink->messages.empty());
}

TEST_CASE(test_sink_level_filter) {
    auto sink = std::make_shared<TestSink>();
    sink->setLevel(RST::Log::LogLevel::Error);
    RST::Log::Logger logger("SinkLvl", RST::Log::LogLevel::Debug);
    logger.addSink(sink);

    logger.debug("filtered by sink");
    logger.warn("also filtered by sink");
    logger.error("passes sink filter");

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0].level == RST::Log::LogLevel::Error);
}
