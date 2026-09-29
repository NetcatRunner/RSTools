#include <RST/RST.hpp>

#include <string>
#include <vector>

// RSTools 2 defines no unprefixed macro unless asked to: LOG_INFO and LOG_DEBUG would
// collide with <syslog.h>.
#if defined(LOG_INFO) || defined(LOG_DEBUG) || defined(LOG_SCOPE_TIME)
#  error "the legacy LOG_* macros must be opt-in (RST_LOG_LEGACY_MACROS)"
#endif

namespace {

    class KeepingSink : public RST::Log::ASink {
    public:
        std::vector<RST::Log::LogMessageBuffer> messages;
    protected:
        void log(const RST::Log::LogMessage& message) override { messages.emplace_back(message); }
    };

}

// Two timers in one scope: the name is pasted with the line number, which used to stay
// the literal text "__LINE__" and redeclare the same variable.
TEST_CASE(test_two_scope_timers_in_one_scope) {
    auto sink = std::make_shared<KeepingSink>();
    auto logger = std::make_shared<RST::Log::Logger>("Timers", RST::Log::LogLevel::Debug);
    logger->addSink(sink);
    {
        RST_LOG_SCOPE_TIME(logger, "outer");
        RST_LOG_SCOPE_TIME(logger, "inner");
    }
    CHECK(sink->messages.size() == 2U);
}

TEST_CASE(test_logger_macros_capture_the_source) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Macros");
    logger.addSink(sink);

    RST_LOGGER_INFO(logger, "at {}", "here");
    RST_LOGGER_ERROR(&logger, "through a pointer");

    CHECK(sink->messages.size() == 2U);
    CHECK(sink->messages[0]->message == "at here");
    CHECK(sink->messages[0]->source.valid());
    CHECK(std::string_view(sink->messages[0]->source.file).find("macros_tests.cpp") != std::string_view::npos);
    CHECK(sink->messages[1]->level == RST::Log::LogLevel::Error);
}

// The default-logger macros go through a reference, not a copied shared_ptr.
TEST_CASE(test_default_logger_macros) {
    RST::Log::Registry::reset();
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Registry::addGlobalSink(sink);

    RST_LOG_WARN("default {}", 1);
    RST_LOG(RST::Log::LogLevel::Error, "default {}", 2);

    CHECK(sink->messages.size() == 2U);
    CHECK(sink->messages[0]->category == "default");
    RST::Log::Registry::reset();
}

// init() twice used to add a second console, so every line was printed twice.
TEST_CASE(test_registry_init_is_idempotent) {
    RST::Log::Registry::reset();
    auto first = RST::Log::Registry::init("app", RST::Log::LogLevel::Info);
    auto second = RST::Log::Registry::init("app", RST::Log::LogLevel::Warn);

    CHECK(first.get() == second.get());
    CHECK(second->sinkCount() == 1U);
    CHECK(second->getLevel() == RST::Log::LogLevel::Warn);
    RST::Log::Registry::reset();
}
