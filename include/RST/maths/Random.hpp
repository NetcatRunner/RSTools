#pragma once

#include <random>
#include <vector>

namespace RST::Maths {

    template<typename T>
    T Random(const T &min, const T &max) {
        static std::random_device dev;
        static std::mt19937 rng(dev());

        if constexpr (std::is_integral<T>::value) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(rng);

        } else if (std::is_floating_point<T>::value) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(rng);
        } else {
            return T{};
        }
    }

    template<typename T>
    T RandomChoice(const std::vector<T>& list) {
        if (list.empty())
            return T{};
        static std::random_device dev;
        static std::mt19937 rng(dev());

        std::uniform_int_distribution<size_t> dist(0, list.size() - 1);
        return list[dist(rng)];

    }
    
}