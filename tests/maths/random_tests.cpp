#include <RST/RST.hpp>

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <thread>
#include <vector>

using RST::Maths::random;
using RST::Maths::randomChoice;
using RST::Maths::randomEngine;

TEST_CASE(test_random_integer_bounds) {
    std::vector<int> counts(6, 0);
    int outside = 0;
    for (int i = 0; i < 6000; ++i) {
        const int value = random(1, 6);
        if (value < 1 || value > 6) {
            ++outside;
            continue;
        }
        ++counts[static_cast<std::size_t>(value - 1)];
    }
    CHECK(outside == 0);
    CHECK(*std::min_element(counts.begin(), counts.end()) > 800);
}

// char and the 8-bit types were undefined behaviour with std::uniform_int_distribution.
TEST_CASE(test_random_small_integer_types) {
    int outside = 0;
    bool sawZero = false;
    bool saw255 = false;
    for (int i = 0; i < 5000; ++i) {
        const char letter = random('a', 'z');
        outside += (letter < 'a' || letter > 'z') ? 1 : 0;

        const std::int8_t small = random<std::int8_t>(-3, 3);
        outside += (small < -3 || small > 3) ? 1 : 0;

        const std::uint8_t byte = random<std::uint8_t>(0, 255);
        sawZero = sawZero || byte == 0;
        saw255 = saw255 || byte == 255;
    }
    CHECK(outside == 0);
    CHECK(sawZero);
    CHECK(saw255);
}

TEST_CASE(test_random_real) {
    int outside = 0;
    for (int i = 0; i < 5000; ++i) {
        const double value = random(0.0, 1.0);
        outside += (value < 0.0 || value >= 1.0) ? 1 : 0;

        const float scaled = random(-2.f, 2.f);
        outside += (scaled < -2.f || scaled > 2.f) ? 1 : 0;
    }
    CHECK(outside == 0);
}

// Seeding the engine replays the same numbers.
TEST_CASE(test_random_seed_replays) {
    std::vector<int> first;
    std::vector<int> second;
    randomEngine().seed(42);
    for (int i = 0; i < 20; ++i) {
        first.push_back(random(0, 1000));
    }
    randomEngine().seed(42);
    for (int i = 0; i < 20; ++i) {
        second.push_back(random(0, 1000));
    }
    CHECK(first == second);
}

TEST_CASE(test_random_choice) {
    const std::vector<std::string> names = {"ann", "bob", "cid"};
    const std::string picked = randomChoice(names);
    CHECK(std::find(names.begin(), names.end(), picked) != names.end());

    CHECK(randomChoice(std::vector<int>{7}) == 7);
    CHECK(randomChoice(std::vector<int>{}) == 0);
}

// Each thread has its own engine: randomChoice's shared static engine was a data race.
TEST_CASE(test_random_engine_is_per_thread) {
    const std::mt19937* otherEngine = nullptr;
    std::thread worker([&] { otherEngine = &randomEngine(); });
    worker.join();
    CHECK(otherEngine != &randomEngine());

    std::vector<int> values = {1, 2, 3, 4, 5};
    std::shuffle(values.begin(), values.end(), randomEngine());
    std::sort(values.begin(), values.end());
    CHECK((values == std::vector<int>{1, 2, 3, 4, 5}));
}
