#pragma once

#include <numbers>

namespace RST::Maths {

    /// @name Constants
    /// Multiply by `DegToRad` or `RadToDeg` to convert an angle.
    /// @{
    inline constexpr double Pi = std::numbers::pi;
    inline constexpr double TwoPi = 2.0 * std::numbers::pi;
    inline constexpr double HalfPi = std::numbers::pi / 2.0;
    inline constexpr double E = std::numbers::e;
    inline constexpr double Sqrt2 = std::numbers::sqrt2;
    inline constexpr double DegToRad = std::numbers::pi / 180.0;
    inline constexpr double RadToDeg = 180.0 / std::numbers::pi;
    /// @}
}
