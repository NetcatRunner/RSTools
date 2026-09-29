#pragma once

#include <array>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

namespace RST::Color {

    // Hue in degrees [0, 360), saturation, value and alpha in [0, 1].
    struct Hsv {
        float h = 0.f;
        float s = 0.f;
        float v = 0.f;
        float a = 1.f;
    };

    // 8-bit RGBA
    struct Color {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;

        constexpr Color() noexcept : r(0), g(0), b(0), a(255) {}
        constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255) noexcept : r(red), g(green), b(blue), a(alpha) {}
        constexpr Color(std::uint32_t hex) noexcept : r(static_cast<std::uint8_t>(hex >> 24)), g(static_cast<std::uint8_t>(hex >> 16)), b(static_cast<std::uint8_t>(hex >> 8)), a(static_cast<std::uint8_t>(hex)) {}

        // ── Construction ─────────────────────────────────────────────────────
        [[nodiscard]] static constexpr Color fromRgb(std::uint32_t rgb, std::uint8_t alpha = 255) noexcept {
            return Color((rgb << 8) | alpha);
        }

        [[nodiscard]] static constexpr Color fromFloat(float red, float green, float blue, float alpha = 1.f) noexcept {
            return Color(toByte(red), toByte(green), toByte(blue), toByte(alpha));
        }

        [[nodiscard]] static Color fromHsv(const Hsv& hsv) noexcept;

        [[nodiscard]] static constexpr std::optional<Color> fromHexString(std::string_view text) noexcept {
            if (!text.empty() && text.front() == '#') {
                text.remove_prefix(1);
            }
            std::uint32_t value = 0;
            for (const char c : text) {
                const int digit = hexDigit(c);
                if (digit < 0) {
                    return std::nullopt;
                }
                value = (value << 4) | static_cast<std::uint32_t>(digit);
            }
            const auto nibble = [value](int index) { return static_cast<std::uint8_t>(((value >> (4 * index)) & 0xF) * 0x11); };
            switch (text.size()) {
                case 3: return Color(nibble(2), nibble(1), nibble(0));
                case 4: return Color(nibble(3), nibble(2), nibble(1), nibble(0));
                case 6: return fromRgb(value);
                case 8: return Color(value);
                default: return std::nullopt;
            }
        }

        // ── Conversion ───────────────────────────────────────────────────────
        [[nodiscard]] constexpr std::uint32_t toHex() const noexcept {
            return (static_cast<std::uint32_t>(r) << 24) | (static_cast<std::uint32_t>(g) << 16) | (static_cast<std::uint32_t>(b) << 8) | a;
        }
        [[nodiscard]] constexpr std::uint32_t toRgb() const noexcept {
            return toHex() >> 8;
        }
        [[nodiscard]] constexpr std::array<float, 4> toFloat() const noexcept {
            return {r / 255.f, g / 255.f, b / 255.f, a / 255.f};
        }

        [[nodiscard]] Hsv toHsv() const noexcept;
        [[nodiscard]] std::string toHexString(bool withAlpha = true) const;

        // ── Variations ───────────────────────────────────────────────────────
        [[nodiscard]] constexpr Color withAlpha(std::uint8_t alpha) const noexcept { return Color(r, g, b, alpha); }

        [[nodiscard]] constexpr Color inverted() const noexcept {
            return Color(static_cast<std::uint8_t>(255 - r), static_cast<std::uint8_t>(255 - g), static_cast<std::uint8_t>(255 - b), a);
        }

        [[nodiscard]] constexpr Color grayscale() const noexcept {
            const auto luma = static_cast<std::uint8_t>((54 * r + 183 * g + 19 * b + 128) >> 8);
            return Color(luma, luma, luma, a);
        }

        [[nodiscard]] constexpr bool operator==(const Color&) const noexcept = default;

    private:
        [[nodiscard]] static constexpr std::uint8_t toByte(float value) noexcept {
            const float clamped = value > 0.f ? (value < 1.f ? value : 1.f) : 0.f;
            return static_cast<std::uint8_t>(clamped * 255.f + 0.5f);
        }

        [[nodiscard]] static constexpr int hexDigit(char c) noexcept {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }
    };

    static_assert(sizeof(Color) == 4, "Color must stay 4 packed bytes");

    std::ostream& operator<<(std::ostream& out, const Color& color);

    constexpr Color Black {0, 0, 0, 255};
    constexpr Color White {255, 255, 255, 255};
    constexpr Color Gray {128, 128, 128, 255};
    constexpr Color Red {255, 0, 0, 255};
    constexpr Color Green {0, 255, 0, 255};
    constexpr Color Blue {0, 0, 255, 255};
    constexpr Color Yellow {255, 255, 0, 255};
    constexpr Color Magenta {255, 0, 255, 255};
    constexpr Color Cyan {0, 255, 255, 255};
    constexpr Color Orange {255, 165, 0, 255};
    constexpr Color Purple {128, 0, 128, 255};
    constexpr Color Transparent {0, 0, 0, 0};

    [[nodiscard]] constexpr Color lerp(const Color& a, const Color& b, float t) noexcept {
        t = t > 0.f ? (t < 1.f ? t : 1.f) : 0.f;
        const auto mix = [t](std::uint8_t from, std::uint8_t to) {
            const float start = static_cast<float>(from);
            return static_cast<std::uint8_t>(start + (static_cast<float>(to) - start) * t + 0.5f);
        };
        return Color(mix(a.r, b.r), mix(a.g, b.g), mix(a.b, b.b), mix(a.a, b.a));
    }

    [[nodiscard]] constexpr Color blend(const Color& src, const Color& dst) noexcept {
        if (src.a == 255 || dst.a == 0) {
            return src;
        }
        if (src.a == 0) {
            return dst;
        }
        const float srcAlpha = static_cast<float>(src.a) / 255.f;
        const float dstAlpha = static_cast<float>(dst.a) / 255.f * (1.f - srcAlpha);
        const float outAlpha = srcAlpha + dstAlpha;
        const auto mix = [=](std::uint8_t s, std::uint8_t d) {
            return static_cast<std::uint8_t>((static_cast<float>(s) * srcAlpha + static_cast<float>(d) * dstAlpha) / outAlpha + 0.5f);
        };
        return Color(mix(src.r, dst.r), mix(src.g, dst.g), mix(src.b, dst.b), static_cast<std::uint8_t>(outAlpha * 255.f + 0.5f));
    }
}
