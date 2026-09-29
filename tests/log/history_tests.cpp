#include <RST/RST.hpp>

#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace {

    class KeepingSink : public RST::Log::ASink {
    public:
        std::vector<RST::Log::LogMessageBuffer> messages;
    protected:
        void log(const RST::Log::LogMessage& message) override { messages.emplace_back(message); }
    };

    class NumberSink : public RST::Log::ASink {
    public:
        std::vector<int> numbers;
    protected:
        void log(const RST::Log::LogMessage& message) override { numbers.push_back(std::stoi(std::string(message.message))); }
    };

}

TEST_CASE(test_ring_buffer_keeps_the_most_recent_messages) {
    auto history = std::make_shared<RST::Log::RingBufferSink>(3);
    RST::Log::Logger logger("History");
    logger.addSink(history);

    for (int i = 0; i < 5; ++i) {
        logger.info("message {}", i);
    }

    const std::vector<RST::Log::LogMessageBuffer> kept = history->snapshot();
    CHECK(kept.size() == 3U);
    CHECK(kept[0]->message == "message 2");
    CHECK(kept[2]->message == "message 4");
}

// Bounded by bytes too: a few huge messages cannot hold megabytes. The newest always stays.
TEST_CASE(test_ring_buffer_is_bounded_by_bytes) {
    auto history = std::make_shared<RST::Log::RingBufferSink>(1000, 4096);
    RST::Log::Logger logger("Bytes");
    logger.addSink(history);

    const std::string big(3000, 'x');
    logger.info("{}", big);
    logger.info("{}", big);
    logger.info("{}", big);

    CHECK(history->size() == 1U);

    const std::string huge(10'000, 'y');
    logger.info("{}", huge);
    CHECK(history->size() == 1U);
    CHECK(history->snapshot()[0]->message.size() == huge.size());
}

// A sink added with the history first receives what came before it, oldest first, then
// the live messages.
TEST_CASE(test_add_sink_replays_the_history_then_goes_live) {
    auto history = std::make_shared<RST::Log::RingBufferSink>(100);
    RST::Log::Logger logger("Replay");
    logger.addSink(history);

    logger.info("before 1");
    logger.warn("before 2");

    auto late = std::make_shared<KeepingSink>();
    logger.addSink(late, *history);
    logger.error("after");

    CHECK(late->messages.size() == 3U);
    CHECK(late->messages[0]->message == "before 1");
    CHECK(late->messages[1]->level == RST::Log::LogLevel::Warn);
    CHECK(late->messages[2]->message == "after");
}

// The replay honours the new sink's own level.
TEST_CASE(test_replay_honours_the_sink_level) {
    auto history = std::make_shared<RST::Log::RingBufferSink>(100);
    RST::Log::Logger logger("Levels");
    logger.addSink(history);

    logger.debug("quiet");
    logger.error("loud");

    auto late = std::make_shared<KeepingSink>();
    late->setLevel(RST::Log::LogLevel::Warn);
    logger.addSink(late, *history);

    CHECK(late->messages.size() == 1U);
    CHECK(late->messages[0]->message == "loud");
}

// While another thread logs a numbered sequence, a sink added with the history receives a
// sequence without a gap or a repeat: the history and the live messages meet exactly. The
// history is sized to keep everything, so the sequence must also start at 0.
TEST_CASE(test_add_sink_with_history_loses_and_repeats_nothing) {
    auto history = std::make_shared<RST::Log::RingBufferSink>(10'000'000, std::size_t{1} << 30);
    RST::Log::Logger logger("Seam");
    logger.addSink(history);

    std::atomic<bool> running{true};
    std::thread writer([&logger, &running] {
        for (int i = 0; running.load(); ++i) {
            logger.info("{}", i);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto late = std::make_shared<NumberSink>();
    logger.addSink(late, *history);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    running.store(false);
    writer.join();

    bool contiguous = !late->numbers.empty() && late->numbers.front() == 0;
    for (std::size_t i = 1; contiguous && i < late->numbers.size(); ++i) {
        contiguous = late->numbers[i] == late->numbers[i - 1] + 1;
    }
    CHECK(contiguous);
}

// Only Windows has a debugger output channel; elsewhere the sink says so and stays silent.
TEST_CASE(test_debugger_sink_is_harmless_where_unsupported) {
    auto debugger = std::make_shared<RST::Log::DebuggerSink>();
    RST::Log::Logger logger("Debugger");
    logger.addSink(debugger);
    logger.info("to the debugger, if one listens");

#if defined(_WIN32)
    CHECK(RST::Log::DebuggerSink::isSupported());
#else
    CHECK(!RST::Log::DebuggerSink::isSupported());
#endif
}
