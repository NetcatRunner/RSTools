#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>
#include <type_traits>

namespace RST::Maths {

    /// @name Numeric helpers
    /// @{
    template <std::floating_point T>
    [[nodiscard]] constexpr T toRad(T degrees) noexcept {
        return degrees * (std::numbers::pi_v<T> / T(180));
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr T toDeg(T radians) noexcept {
        return radians * (T(180) / std::numbers::pi_v<T>);
    }

    template <typename T>
    [[nodiscard]] constexpr T abs(T value) noexcept {
        return (value < T(0)) ? -value : value;
    }

    template <typename T>
    [[nodiscard]] constexpr T clamp(T value, T min, T max) noexcept {
        return (value < min) ? min : ((value > max) ? max : value);
    }

    /// Returns -1, 0 or 1 depending on the sign of `value`.
    template <typename T>
    [[nodiscard]] constexpr int sign(T value) noexcept {
        return (T(0) < value) - (value < T(0));
    }

    /// Linear interpolation from `start` to `end`, with `t` clamped to [0, 1].
    template <typename T, typename U>
    [[nodiscard]] constexpr T lerp(T start, T end, U t) noexcept {
        return start + (end - start) * clamp(t, U(0), U(1));
    }

    /// Inverse of lerp(): where `value` lies between `start` and `end`, or 0 if they are equal.
    template <std::floating_point T>
    [[nodiscard]] constexpr T inverseLerp(T start, T end, T value) noexcept {
        return start == end ? T(0) : (value - start) / (end - start);
    }

    /// Maps `value` from [`inMin`, `inMax`] to [`outMin`, `outMax`], without clamping.
    template <typename T>
    [[nodiscard]] constexpr T remap(T value, T inMin, T inMax, T outMin, T outMax) noexcept {
        return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
    }

    /// Smooth Hermite interpolation from 0 at `edge0` to 1 at `edge1`.
    template <std::floating_point T>
    [[nodiscard]] constexpr T smoothstep(T edge0, T edge1, T x) noexcept {
        const T t = clamp(inverseLerp(edge0, edge1, x), T(0), T(1));
        return t * t * (T(3) - T(2) * t);
    }

    /// Wraps `value` into [`min`, `max`), such as an angle into [0, 360).
    template <typename T> requires std::is_signed_v<T>
    [[nodiscard]] T wrap(T value, T min, T max) noexcept {
        const T range = max - min;
        if constexpr (std::is_floating_point_v<T>) {
            return value - range * std::floor((value - min) / range);
        } else {
            const T offset = static_cast<T>((value - min) % range);
            return static_cast<T>((offset < 0 ? offset + range : offset) + min);
        }
    }

    /// True if `a` and `b` differ by at most `epsilon`, scaled by their magnitude when it exceeds 1.
    template <typename T>
    [[nodiscard]] constexpr bool isNearlyEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) noexcept {
        if (a == b) {
            return true;
        }
        const T largest = abs(a) > abs(b) ? abs(a) : abs(b);
        if constexpr (std::numeric_limits<T>::has_infinity) {
            if (largest > std::numeric_limits<T>::max()) {
                return false;
            }
        }
        return abs(a - b) <= epsilon * (largest > T(1) ? largest : T(1));
    }
    /// @}

}
