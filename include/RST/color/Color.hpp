#pragma once

#include <cstdint>

namespace RST::Color {
    struct Color {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    
        constexpr Color() : r(0), g(0), b(0), a(255) {}
        constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255): r(red), g(green), b(blue), a(alpha) {};
        constexpr Color(std::uint32_t hex): r((hex >> 24) & 0xFF), g((hex >> 16) & 0xFF), b((hex >> 8) & 0xFF), a(hex & 0xFF) {};
    
        constexpr bool operator==(const Color& other) const {return r == other.r && g == other.g && b == other.b && a == other.a;};
        constexpr bool operator!=(const Color& other) const {return !(*this == other);};
    
        constexpr std::uint32_t toHex() const {return (r << 24) | (g << 16) | (b << 8) | a;};
    };

    constexpr Color Black       {0, 0, 0, 255};
    constexpr Color White       {255, 255, 255, 255};
    constexpr Color Red         {255, 0, 0, 255};
    constexpr Color Green       {0, 255, 0, 255};
    constexpr Color Blue        {0, 0, 255, 255};
    constexpr Color Yellow      {255, 255, 0, 255};
    constexpr Color Magenta     {255, 0, 255, 255};
    constexpr Color Cyan        {0, 255, 255, 255};
    constexpr Color Transparent {0, 0, 0, 0};
}

