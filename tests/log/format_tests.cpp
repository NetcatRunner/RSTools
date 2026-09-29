#include <RST/RST.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace {

    class KeepingSink : public RST::Log::ASink {
    public:
        std::vector<RST::Log::LogMessageBuffer> messages;
    protected:
        void log(const RST::Log::LogMessage& message) override { messages.emplace_back(message); }
    };

    struct Throwing {};

}

template <>
struct std::formatter<Throwing> : std::formatter<std::string_view> {
    auto format(const Throwing&, std::format_context&) const -> std::format_context::iterator
    {
        throw std::runtime_error("formatter exploded");
    }
};

// std::format, not a positional substitution: the specifiers are honoured.
TEST_CASE(test_format_specifiers_are_honoured) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Specs");
    logger.addSink(sink);

    logger.info("{:.2f} | {:>5} | {:#x} | {{literal}}", 16.6667, 42, 255);

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0]->message == "16.67 |    42 | 0xff | {literal}");
}

// A message longer than the inline buffer grows onto the heap; nothing is cut.
TEST_CASE(test_long_message_is_not_truncated) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Long");
    logger.addSink(sink);

    const std::string payload(100'000, 'x');
    logger.info("[{}]", payload);

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0]->message.size() == payload.size() + 2);
    CHECK(sink->messages[0]->message.back() == ']');
}

// A text known only at run time is written as is: its braces are not placeholders.
TEST_CASE(test_runtime_text_keeps_its_braces) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Runtime");
    logger.addSink(sink);

    const std::string text = "set {a, b} = {}";
    logger.warn(text);
    logger.log(RST::Log::LogLevel::Error, std::string_view(text));

    CHECK(sink->messages.size() == 2);
    CHECK(sink->messages[0]->message == text);
    CHECK(sink->messages[1]->message == text);
}

// A formatter that throws costs the message's content, never the line nor the caller.
TEST_CASE(test_throwing_formatter_does_not_escape) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Throwing");
    logger.addSink(sink);

    logger.error("value = {}", Throwing{});

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0]->message.find("formatter exploded") != std::string_view::npos);
    CHECK(sink->messages[0]->level == RST::Log::LogLevel::Error);
}

// Below the level, the arguments are not even evaluated: the macro tests the level first.
TEST_CASE(test_filtered_macro_evaluates_nothing) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Lazy", RST::Log::LogLevel::Warn);
    logger.addSink(sink);

    int evaluations = 0;
    const auto counted = [&evaluations] { return ++evaluations; };

    RST_LOGGER_DEBUG(logger, "value {}", counted());
    RST_LOGGER_WARN(logger, "value {}", counted());

    CHECK(evaluations == 1);
    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0]->source.valid());
}

// The category of a message can differ from the logger's name: that is how a facade
// (an engine with its own categories) routes everything through one logger.
TEST_CASE(test_vlog_carries_its_own_category) {
    auto sink = std::make_shared<KeepingSink>();
    RST::Log::Logger logger("Engine");
    logger.addSink(sink);

    const int frames = 3;
    logger.vlog(RST::Log::LogLevel::Info, RST_SOURCE_LOCATION, "Renderer", "{} frames", std::make_format_args(frames));

    CHECK(sink->messages.size() == 1);
    CHECK(sink->messages[0]->category == "Renderer");
    CHECK(sink->messages[0]->message == "3 frames");
}

TEST_CASE(test_thread_token_is_produced) {
    RST::Log::Formatter formatter("%t");
    const RST::Log::LogMessage message(RST::Log::LogLevel::Info, "x", "y");
    const std::string out = formatter.format(message);
    CHECK(!out.empty());
    CHECK(out != "%t");
    CHECK(out == std::to_string(message.threadId));
}

// A kept copy owns its text: it survives the buffer it was made from, and so do its copies.
TEST_CASE(test_message_buffer_owns_its_text) {
    std::vector<RST::Log::LogMessageBuffer> kept;
    {
        std::string category = "Temporary";
        std::string text = "short";
        const RST::Log::SourceLocation source{"dir/file.cpp", "function", 12};
        kept.emplace_back(RST::Log::LogMessage(RST::Log::LogLevel::Info, category, text, source));
        category.assign(64, '#');
        text.assign(64, '#');
    }
    const RST::Log::LogMessageBuffer copy = kept[0];
    RST::Log::LogMessageBuffer moved = std::move(kept[0]);

    CHECK(copy->category == "Temporary");
    CHECK(copy->message == "short");
    CHECK(std::string_view(copy->source.file) == "dir/file.cpp");
    CHECK(moved->message == "short");
    CHECK(moved->source.line == 12);
}

static_assert(RST::Log::to_string(RST::Log::LogLevel::Warn) == "WARN");
static_assert(RST::Log::to_short_string(RST::Log::LogLevel::Fatal) == "FTL");
