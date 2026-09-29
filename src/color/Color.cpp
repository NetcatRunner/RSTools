#include "RST/color/Color.hpp"

#include <cmath>
#include <ostream>

namespace RST::Color {

    Color Color::fromHsv(const Hsv& hsv) noexcept
    {
        const float s = hsv.s > 0.f ? (hsv.s < 1.f ? hsv.s : 1.f) : 0.f;
        const float v = hsv.v > 0.f ? (hsv.v < 1.f ? hsv.v : 1.f) : 0.f;
        float h = std::fmod(hsv.h, 360.f);
        if (!(h >= 0.f)) {
            h = h < 0.f ? h + 360.f : 0.f;
        }

        const float sector = h / 60.f;
        const int index = static_cast<int>(sector) % 6;
        const float f = sector - static_cast<float>(static_cast<int>(sector));
        const float p = v * (1.f - s);
        const float q = v * (1.f - s * f);
        const float t = v * (1.f - s * (1.f - f));

        switch (index) {
            case 0:  return fromFloat(v, t, p, hsv.a);
            case 1:  return fromFloat(q, v, p, hsv.a);
            case 2:  return fromFloat(p, v, t, hsv.a);
            case 3:  return fromFloat(p, q, v, hsv.a);
            case 4:  return fromFloat(t, p, v, hsv.a);
            default: return fromFloat(v, p, q, hsv.a);
        }
    }

    Hsv Color::toHsv() const noexcept
    {
        const auto [red, green, blue, alpha] = toFloat();
        const float max = std::fmax(red, std::fmax(green, blue));
        const float min = std::fmin(red, std::fmin(green, blue));
        const float delta = max - min;

        Hsv hsv{0.f, max > 0.f ? delta / max : 0.f, max, alpha};
        if (delta > 0.f) {
            if (max == red) {
                hsv.h = 60.f * ((green - blue) / delta);
            } else if (max == green) {
                hsv.h = 60.f * ((blue - red) / delta + 2.f);
            } else {
                hsv.h = 60.f * ((red - green) / delta + 4.f);
            }
            if (hsv.h < 0.f) {
                hsv.h += 360.f;
            }
        }
        return hsv;
    }

    std::string Color::toHexString(bool withAlpha) const
    {
        constexpr char digits[] = "0123456789ABCDEF";
        std::string text(withAlpha ? 9 : 7, '#');
        const std::uint8_t channels[] = {r, g, b, a};
        for (std::size_t i = 0; i < (withAlpha ? 4u : 3u); ++i) {
            text[1 + 2 * i] = digits[channels[i] >> 4];
            text[2 + 2 * i] = digits[channels[i] & 0xF];
        }
        return text;
    }

    std::ostream& operator<<(std::ostream& out, const Color& color)
    {
        return out << color.toHexString();
    }
}
