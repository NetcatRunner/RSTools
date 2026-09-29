#include <RST/RST.hpp>

#include <chrono>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using RST::Time::Timer;

// Never started: zero, not the time since the clock's epoch.
TEST_CASE(test_timer_not_started_is_zero) {
    const Timer timer;
    CHECK(!timer.isRunning());
    CHECK(timer.elapsed() == Timer::Duration::zero());
    CHECK(timer.getElapsedMs() == 0.0);
}

TEST_CASE(test_timer_stop_freezes_and_resume_accumulates) {
    Timer timer;
    timer.start();
    std::this_thread::sleep_for(5ms);
    timer.stop();
    const Timer::Duration first = timer.elapsed();
    CHECK(first >= 5ms);

    std::this_thread::sleep_for(5ms);
    CHECK(timer.elapsed() == first);

    timer.resume();
    std::this_thread::sleep_for(5ms);
    timer.stop();
    CHECK(timer.elapsed() >= first + 5ms);

    timer.start();
    CHECK(timer.elapsed() < first);

    timer.reset();
    CHECK(!timer.isRunning());
    CHECK(timer.elapsed() == Timer::Duration::zero());
}

// Integer results work too (they were a compile error: no implicit truncating conversion).
TEST_CASE(test_timer_units) {
    Timer timer = Timer::startNew();
    CHECK(timer.isRunning());
    std::this_thread::sleep_for(3ms);
    timer.stop();
    CHECK(timer.getElapsedMs<long long>() >= 3);
    CHECK(timer.getElapsedUs<long long>() >= 3000);
    CHECK(timer.getElapsedNs<long long>() >= 3'000'000);
    CHECK(timer.getElapsedSeconds() >= 0.003);
    CHECK(timer.getElapsedSeconds<float>() < 10.f);
}

TEST_CASE(test_timer_restart_returns_lap) {
    Timer timer = Timer::startNew();
    std::this_thread::sleep_for(3ms);
    const Timer::Duration lap = timer.restart();
    CHECK(lap >= 3ms);
    CHECK(timer.isRunning());
    CHECK(timer.elapsed() < lap);
}

TEST_CASE(test_frame_timer_caps_frame_rate) {
    RST::Time::FrameTimer frames(100);
    CHECK(frames.getTargetFps() == 100u);
    CHECK(frames.getFrameCount() == 0u);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; ++i) {
        frames.tick();
        CHECK(frames.getDeltaTime() >= 0.0099);
    }
    const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

    CHECK(frames.getFrameCount() == 10u);
    CHECK(seconds >= 0.099);
    CHECK(seconds < 0.2);
    CHECK(frames.getAverageFps() > 80.0);
    CHECK(frames.getAverageFps() <= 101.0);
    CHECK(frames.getCurrentFps() <= 101.0);
    CHECK(frames.getTotalTime() >= 0.099);
}

TEST_CASE(test_frame_timer_uncapped_only_measures) {
    RST::Time::FrameTimer frames;
    CHECK(frames.getTargetFps() == 0u);
    CHECK(frames.getCurrentFps() == 0.0);

    const auto start = std::chrono::steady_clock::now();
    frames.tick();
    frames.tick();
    CHECK(std::chrono::steady_clock::now() - start < 5ms);

    frames.setTargetFps(50);
    frames.tick();
    CHECK(frames.getDeltaTime() >= 0.0199);
}

TEST_CASE(test_format_time) {
    const std::chrono::system_clock::time_point epoch{};
    CHECK(RST::Time::formatTime(epoch, "%Y-%m-%d %H:%M:%S", RST::Time::TimeZone::Utc) == "1970-01-01 00:00:00");

    const auto moment = std::chrono::system_clock::time_point(std::chrono::seconds(1'700'000'000));
    CHECK(RST::Time::formatTime(moment, "%Y%m%d-%H%M%S", RST::Time::TimeZone::Utc) == "20231114-221320");
    CHECK(RST::Time::formatTime(moment, "", RST::Time::TimeZone::Utc) == "");

    // Local time: the zone of the machine is unknown, the shape is not.
    CHECK(RST::Time::formatNow().size() == 19u);
}

// strftime reports a too small buffer as 0: the buffer grows until the text fits.
TEST_CASE(test_format_time_long_output) {
    std::string format;
    for (int i = 0; i < 100; ++i) {
        format += "%Y-%m-%d ";
    }
    const std::string text = RST::Time::formatTime(std::chrono::system_clock::time_point{}, format, RST::Time::TimeZone::Utc);
    CHECK(text.size() == 100u * 11u);
    CHECK(text.substr(0, 11) == "1970-01-01 ");
}
