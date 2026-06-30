#pragma once

#include <cmath>

namespace RST::Maths {

    template <typename T>
    constexpr T toRad(T degrees) noexcept {
        return degrees * (T(M_PI) / T(180));
    }

    template <typename T>
    constexpr T toDeg(T radians) noexcept {
        return radians * (T(180) / T(M_PI));
    }

    template <typename T>
    constexpr T abs(T value) noexcept {
        return (value < T(0)) ? -value : value;
    }

    template <typename T>
    constexpr T clamp(T value, T min, T max) noexcept {
        return (value < min) ? min : ((value > max) ? max : value);
    }

    template <typename T>
    constexpr int sign(T value) noexcept {
        return (T(0) < value) - (value < T(0));
    }

    template <typename T, typename U>
    constexpr T lerp(T start, T end, U t) noexcept {
        return start + (end - start) * clamp(t, U(0), U(1));
    }

    template <typename T>
    constexpr T remap(T value, T inMin, T inMax, T outMin, T outMax) noexcept {
        return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
    }

    template <typename T>
    constexpr bool isNearlyEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) noexcept {
        return abs(a - b) <= epsilon;
    }
    
}
