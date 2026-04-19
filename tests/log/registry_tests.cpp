#include <RST/RST.hpp>

class TestSink : public RST::Log::ASink {
public:
    std::vector<RST::Log::LogMessage> messages;
protected:
    void log(const RST::Log::LogMessage& msg) override {
        messages.push_back(msg);
    }
};

TEST_CASE(test_registry_get_creates_logger) {
    RST::Log::Registry::reset();
    auto a = RST::Log::Registry::get("ModuleA");
    auto b = RST::Log::Registry::get("ModuleB");
    CHECK(a != nullptr);
    CHECK(b != nullptr);
    CHECK(a.get() != b.get());
    CHECK(a->getName() == "ModuleA");
}

TEST_CASE(test_registry_get_returns_same) {
    RST::Log::Registry::reset();
    auto a1 = RST::Log::Registry::get("SameModule");
    auto a2 = RST::Log::Registry::get("SameModule");
    CHECK(a1.get() == a2.get());
}

TEST_CASE(test_registry_default_logger) {
    RST::Log::Registry::reset();
    auto d = RST::Log::Registry::defaultLogger();
    CHECK(d != nullptr);
    CHECK(d->getName() == "default");
}

TEST_CASE(test_registry_global_level) {
    RST::Log::Registry::reset();
    auto sink = std::make_shared<TestSink>();
    RST::Log::Registry::addGlobalSink(sink);

    auto log = RST::Log::Registry::get("LvlTest");

    RST::Log::Registry::setGlobalLevel(RST::Log::LogLevel::Error);
    log->info("filtered by global level");
    CHECK(sink->messages.empty());

    log->error("passes");
    CHECK(sink->messages.size() == 1);
}

TEST_CASE(test_registry_global_sink_propagates_to_new_logger) {
    RST::Log::Registry::reset();
    auto sink = std::make_shared<TestSink>();
    RST::Log::Registry::addGlobalSink(sink);

    auto log = RST::Log::Registry::get("NewLogger");
    log->info("hello");
    CHECK(sink->messages.size() == 1);
}

TEST_CASE(test_registry_reset) {
    RST::Log::Registry::reset();
    auto a = RST::Log::Registry::get("Before");

    RST::Log::Registry::reset();
    auto b = RST::Log::Registry::get("Before");

    CHECK(a.get() != b.get());
}

