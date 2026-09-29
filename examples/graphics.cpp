#include <RST/color/Color.hpp>
#include <RST/maths/Maths.hpp>

#include <iostream>

int main() {
    using namespace RST::Maths;
    namespace Colors = RST::Color;

    // Vectors
    const Vec2f position(3.f, 4.f);
    const Vec2f velocity = Vec2f(1.f, 0.5f) * 2;
    std::cout << "distance to origin: " << position.length() << '\n';            // 5
    std::cout << "next position: " << position + velocity << '\n';               // (5, 5)

    // Matrices: scale then move a point
    const Matrix4f transform = Matrix4f::translation(1.f, 2.f, 3.f) * Matrix4f::scale(2.f);
    std::cout << "transformed: " << transform.transformPoint(Vec3f(1.f, 1.f, 1.f)) << '\n';   // (3, 4, 5)

    // Random numbers, reproducible once seeded
    randomEngine().seed(42);
    std::cout << "dice: " << random(1, 6) << ", chance: " << random(0.0, 1.0) << '\n';

    // Colors
    const Colors::Color sky = Colors::Color::fromHexString("#87CEEB").value_or(Colors::Blue);
    const Colors::Color dusk = Colors::lerp(sky, Colors::Orange, 0.5f);
    std::cout << "dusk: " << dusk << ", hue " << dusk.toHsv().h << '\n';
    return 0;
}
