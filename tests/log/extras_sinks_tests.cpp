#include <RST/RST.hpp>

TEST_CASE(test_null_sink_discards) {
    auto sink = std::make_shared<RST::Log::NullSink>();
    RST::Log::Logger logger("NullTest");
    logger.addSink(sink);

    logger.trace("trace");
    logger.info("info");
    logger.fatal("fatal");
    CHECK(true);
}

TEST_CASE(test_ostream_sink_captures) {
    std::ostringstream oss;
    auto sink = std::make_shared<RST::Log::OStreamSink>(oss);
    sink->setPattern("%v");

    RST::Log::Logger logger("OStreamTest");
    logger.addSink(sink);
    logger.info("captured output");

    CHECK(oss.str() == "captured output\n");
}

TEST_CASE(test_ostream_sink_multiple_messages) {
    std::ostringstream oss;
    auto sink = std::make_shared<RST::Log::OStreamSink>(oss);
    sink->setPattern("%v");

    RST::Log::Logger logger("Multi");
    logger.addSink(sink);
    logger.info("first");
    logger.warn("second");
    logger.error("third");

    const auto& s = oss.str();
    CHECK(s.find("first") != std::string::npos);
    CHECK(s.find("second") != std::string::npos);
    CHECK(s.find("third") != std::string::npos);
}

TEST_CASE(test_callback_sink_called) {
    std::vector<RST::Log::LogMessage> captured;
    auto sink = std::make_shared<RST::Log::CallbackSink>([&](const RST::Log::LogMessage& msg) {
        captured.push_back(msg);
    });

    RST::Log::Logger logger("CbTest");
    logger.addSink(sink);
    logger.info("hello callback");
    logger.error("error callback");

    CHECK(captured.size() == 2);
    CHECK(captured[0].level == RST::Log::LogLevel::Info);
    CHECK(captured[1].level == RST::Log::LogLevel::Error);
    CHECK(captured[0].message == "hello callback");
}

TEST_CASE(test_callback_sink_level_filter) {
    int count = 0;
    auto sink = std::make_shared<RST::Log::CallbackSink>([&](const RST::Log::LogMessage&) { ++count; });
    sink->setLevel(RST::Log::LogLevel::Error);

    RST::Log::Logger logger("CbFilter");
    logger.addSink(sink);
    logger.debug("filtered");
    logger.info("filtered");
    logger.warn("filtered");
    logger.error("passes");
    logger.fatal("passes");

    CHECK(count == 2);
}

TEST_CASE(test_callback_sink_replace_callback) {
    int callsA = 0, callsB = 0;
    auto sink = std::make_shared<RST::Log::CallbackSink>([&](const RST::Log::LogMessage&) { ++callsA; });

    RST::Log::Logger logger("CbReplace");
    logger.addSink(sink);
    logger.info("first");

    sink->setCallback([&](const RST::Log::LogMessage&) { ++callsB; });
    logger.info("second");

    CHECK(callsA == 1);
    CHECK(callsB == 1);
}

TEST_CASE(test_scoped_timer_logs_on_exit) {
    std::vector<RST::Log::LogMessage> captured;
    auto sink = std::make_shared<RST::Log::CallbackSink>([&](const RST::Log::LogMessage& msg) {
        captured.push_back(msg);
    });

    auto logger = std::make_shared<RST::Log::Logger>("TimerTest", RST::Log::LogLevel::Debug);
    logger->addSink(sink);

    {
        RST::Log::ScopedTimer timer("my_operation", logger);
    }

    CHECK(captured.size() == 1);
    CHECK(captured[0].message.find("my_operation") != std::string::npos);
    CHECK(captured[0].message.find("ms") != std::string::npos);
}

TEST_CASE(test_scoped_timer_warn_on_slow) {
    std::vector<RST::Log::LogMessage> captured;
    auto sink = std::make_shared<RST::Log::CallbackSink>([&](const RST::Log::LogMessage& msg) {
        captured.push_back(msg);
    });

    auto logger = std::make_shared<RST::Log::Logger>("SlowTimer", RST::Log::LogLevel::Debug);
    logger->addSink(sink);
    {
        RST::Log::ScopedTimer timer("slow_op", logger, std::chrono::milliseconds(0), RST::Log::LogLevel::Debug);
    }

    CHECK(captured.size() == 1);
    CHECK(captured[0].level == RST::Log::LogLevel::Warn);
    CHECK(captured[0].message.find("[SLOW]") != std::string::npos);
}

TEST_CASE(test_scoped_timer_elapsed) {
    auto logger = std::make_shared<RST::Log::Logger>("ElapsedTest", RST::Log::LogLevel::Debug);
    logger->addSink(std::make_shared<RST::Log::NullSink>());

    RST::Log::ScopedTimer timer("measure", logger);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto e = timer.elapsed();
    CHECK(e.count() >= 4);
}
