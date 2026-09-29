#include <RST/RST.hpp>

#include <atomic>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

    class CountingSink : public RST::Log::ASink {
    public:
        std::size_t count = 0;
    protected:
        void log(const RST::Log::LogMessage&) override { ++count; }
    };

    constexpr int kThreads = 8;
    constexpr int kMessagesPerThread = 2000;

}

// Several threads log into the same logger: every message arrives, and a stream sink
// never interleaves two lines. Run the suite under ThreadSanitizer to see the rest.
TEST_CASE(test_concurrent_logging_loses_nothing) {
    auto counting = std::make_shared<CountingSink>();
    std::ostringstream stream;
    auto lines = std::make_shared<RST::Log::OStreamSink>(stream);
    lines->setPattern("%v");

    RST::Log::Logger logger("Threads");
    logger.addSink(counting);
    logger.addSink(lines);

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&logger, t] {
            for (int i = 0; i < kMessagesPerThread; ++i) {
                logger.info("thread {} message {}", t, i);
            }
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    CHECK(counting->count == static_cast<std::size_t>(kThreads * kMessagesPerThread));

    std::istringstream read(stream.str());
    std::size_t lineCount = 0;
    bool wellFormed = true;
    for (std::string line; std::getline(read, line); ++lineCount) {
        wellFormed = wellFormed && line.starts_with("thread ") && line.find(" message ") != std::string::npos;
    }
    CHECK(lineCount == static_cast<std::size_t>(kThreads * kMessagesPerThread));
    CHECK(wellFormed);
}

// Sinks come and go while other threads log: the sink list is guarded, not just the sinks.
TEST_CASE(test_sinks_change_while_logging) {
    RST::Log::Logger logger("Churn");
    std::atomic<bool> running{true};

    std::vector<std::thread> writers;
    for (int t = 0; t < 4; ++t) {
        writers.emplace_back([&logger, &running] {
            while (running.load()) {
                logger.debug("tick");
            }
        });
    }

    for (int round = 0; round < 500; ++round) {
        auto sink = std::make_shared<CountingSink>();
        logger.addSink(sink);
        logger.removeSink(sink);
    }
    running.store(false);
    for (std::thread& writer : writers) {
        writer.join();
    }

    CHECK(logger.sinkCount() == 0);
}

// The level can change while other threads read it.
TEST_CASE(test_level_changes_while_logging) {
    auto sink = std::make_shared<CountingSink>();
    RST::Log::Logger logger("Levels");
    logger.addSink(sink);

    std::thread writer([&logger] {
        for (int i = 0; i < 5000; ++i) {
            logger.info("x");
        }
    });
    for (int i = 0; i < 5000; ++i) {
        logger.setLevel(i % 2 == 0 ? RST::Log::LogLevel::Error : RST::Log::LogLevel::Trace);
    }
    writer.join();

    CHECK(sink->count <= 5000U);
}

// flushOn: a message at or above the level flushes every sink, which is what keeps the
// last line before an abort().
TEST_CASE(test_flush_on_level) {
    class FlushCounter : public RST::Log::ASink {
    public:
        int flushes = 0;
    protected:
        void log(const RST::Log::LogMessage&) override {}
        void flushSink() override { ++flushes; }
    };

    auto sink = std::make_shared<FlushCounter>();
    RST::Log::Logger logger("Flush");
    logger.addSink(sink);
    logger.flushOn(RST::Log::LogLevel::Error);

    logger.info("no flush");
    logger.warn("no flush");
    logger.error("flush");
    logger.fatal("flush");

    CHECK(sink->flushes == 2);
}

TEST_CASE(test_remove_sink) {
    auto kept = std::make_shared<CountingSink>();
    auto removed = std::make_shared<CountingSink>();
    RST::Log::Logger logger("Remove");
    logger.addSink(kept);
    logger.addSink(removed);

    CHECK(logger.removeSink(removed));
    CHECK(!logger.removeSink(removed));
    logger.info("only the kept sink");

    CHECK(kept->count == 1U);
    CHECK(removed->count == 0U);
}
