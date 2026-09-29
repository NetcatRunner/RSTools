#pragma once

#include <cstddef>
#include <random>
#include <type_traits>
#include <vector>

namespace RST::Maths {

    inline std::mt19937& randomEngine() {
        thread_local std::mt19937 engine(std::random_device{}());
        return engine;
    }

    template<typename T>
    T random(const T& min, const T& max) {
        static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, "random needs a number type");

        if constexpr (std::is_integral_v<T>) {
            using Wide = std::conditional_t<std::is_signed_v<T>, long long, unsigned long long>;
            std::uniform_int_distribution<Wide> dist(min, max);
            return static_cast<T>(dist(randomEngine()));
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(randomEngine());
        }
    }

    template<typename T>
    T randomChoice(const std::vector<T>& list) {
        if (list.empty())
            return T{};

        std::uniform_int_distribution<std::size_t> dist(0, list.size() - 1);
        return list[dist(randomEngine())];
    }

}
