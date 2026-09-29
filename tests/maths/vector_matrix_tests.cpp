#include <RST/RST.hpp>

#include <cmath>
#include <limits>
#include <sstream>
#include <type_traits>

using namespace RST::Maths;

static_assert(std::is_trivially_copyable_v<Vec3f>);
static_assert(Vec2i(1, 2) + Vec2i(3, 4) == Vec2i(4, 6));
static_assert(Vec3i(1, 0, 0).cross(Vec3i(0, 1, 0)) == Vec3i(0, 0, 1));
static_assert(2 * Vec2i(1, 2) == Vec2i(2, 4));
static_assert(Vec2i(3, 4).lengthSquared() == 25);

static_assert(!std::is_convertible_v<float, Matrix4f>, "a scalar no longer turns into a matrix silently");
static_assert(Matrix<int, 2, 2>{1, 2, 3, 4}.determinant() == -2);
static_assert(Matrix<int, 3, 3>{2, 0, 0, 0, 3, 0, 0, 0, 4}.determinant() == 24);
static_assert(Matrix4f::identity() == Matrix4f());

// Operators never throw any more: a tiny divisor used to throw std::invalid_argument.
TEST_CASE(test_vector_division_follows_ieee) {
    const Vec2f tiny = Vec2f(1.f, 1.f) / Vec2f(1e-8f, 1.f);
    CHECK(isNearlyEqual(tiny.x, 1e8f));

    const Vec3f byZero = Vec3f(1.f, -1.f, 0.f) / 0.f;
    CHECK(std::isinf(byZero.x));
    CHECK(std::isinf(byZero.y));
    CHECK(std::isnan(byZero.z));
}

TEST_CASE(test_vector_indexing) {
    Vec3f v(1.f, 2.f, 3.f);
    v[1] = 5.f;
    const Vec3f& constRef = v;
    CHECK(constRef[0] == 1.f);
    CHECK(constRef[1] == 5.f);
    CHECK(constRef[2] == 3.f);

    Vec2i w(7, 8);
    w[0] += 1;
    const Vec2i& constW = w;
    CHECK(constW[0] == 8);
    CHECK(constW[1] == 8);
}

// The scalar is not deduced any more: v * 2 compiles for a Vec2f.
TEST_CASE(test_vector_scalar_operators) {
    const Vec2f v(1.5f, -2.f);
    CHECK(v * 2 == Vec2f(3.f, -4.f));
    CHECK(2 * v == Vec2f(3.f, -4.f));
    CHECK(v / 2 == Vec2f(0.75f, -1.f));
    CHECK(v + 1 == Vec2f(2.5f, -1.f));
    CHECK(v - 1 == Vec2f(0.5f, -3.f));
    CHECK(-v == Vec2f(-1.5f, 2.f));
}

TEST_CASE(test_vector_geometry) {
    const Vec2f a(3.f, 4.f);
    CHECK(a.length() == 5.f);
    CHECK(a.lengthSquared() == 25.f);
    CHECK(a.dot(Vec2f(1.f, 0.f)) == 3.f);
    CHECK(Vec2f(1.f, 0.f).cross(Vec2f(0.f, 1.f)) == 1.f);
    CHECK(Vec2f(1.f, 0.f).perpendicular() == Vec2f(0.f, 1.f));
    CHECK(a.distance(Vec2f(0.f, 0.f)) == 5.f);
    CHECK(Vec3f(1.f, 2.f, 2.f).distanceSquared(Vec3f()) == 9.f);
    CHECK(isNearlyEqual(Vec3f(3.f, 1.f, -2.f).normalized().length(), 1.f));
    CHECK(Vec3f().normalized() == Vec3f());
    CHECK(Vec2f(1.f, 5.f).min(Vec2f(2.f, 3.f)) == Vec2f(1.f, 3.f));
    CHECK(Vec3i(1, 5, 0).max(Vec3i(2, 3, 0)) == Vec3i(2, 5, 0));
    CHECK(Vec2f(1.9f, -1.9f).to<int>() == Vec2i(1, -1));
}

TEST_CASE(test_vector_structured_bindings_and_output) {
    const auto [x, y, z] = Vec3i(1, 2, 3);
    CHECK(x + y + z == 6);

    std::ostringstream out;
    out << Vec2i(1, 2) << ' ' << Vec3i(3, 4, 5);
    CHECK(out.str() == "(1, 2) (3, 4, 5)");
}

TEST_CASE(test_matrix_basics) {
    Matrix3f m{1.f, 2.f, 3.f,
               4.f, 5.f, 6.f,
               7.f, 8.f, 10.f};
    CHECK(m.data()[5] == 6.f);
    CHECK(m.transpose()(0, 1) == 4.f);
    CHECK(Matrix3f::rows() == 3u);

    Matrix3f sum = m;
    sum += Matrix3f();
    CHECK(sum(0, 0) == 2.f);
    sum -= Matrix3f();
    CHECK(sum == m);
    sum *= 2.f;
    CHECK(sum(2, 2) == 20.f);

    Matrix3f product = m;
    product *= Matrix3f();
    CHECK(product == m);
}

TEST_CASE(test_matrix_determinant_and_inverse) {
    const Matrix4d m{2.0, 0.0, 0.0, 1.0,
                     0.0, 3.0, 0.0, 2.0,
                     0.0, 0.0, 4.0, 3.0,
                     0.0, 0.0, 0.0, 1.0};
    CHECK(isNearlyEqual(m.determinant(), 24.0));

    const Matrix4d identity = m * m.inverse();
    for (std::size_t r = 0; r < 4; ++r) {
        for (std::size_t c = 0; c < 4; ++c) {
            CHECK(isNearlyEqual(identity(r, c), r == c ? 1.0 : 0.0, 1e-12));
        }
    }

    const Matrix4d singular(1.0);
    CHECK(singular.determinant() == 0.0);

    // A row swap flips the sign.
    const Matrix3d swapped{0.0, 1.0, 0.0,
                           1.0, 0.0, 0.0,
                           0.0, 0.0, 1.0};
    CHECK(swapped.determinant() == -1.0);
}

// perspective() now converts degrees with toRad(); same OpenGL-style result as before.
TEST_CASE(test_matrix_projection_and_transform) {
    const Matrix4f projection = Matrix4f::perspective(90.f, 1.f, 1.f, 3.f);
    CHECK(isNearlyEqual(projection(0, 0), 1.f));
    CHECK(isNearlyEqual(projection(1, 1), 1.f));
    CHECK(isNearlyEqual(projection(2, 2), -2.f));
    CHECK(isNearlyEqual(projection(2, 3), -3.f));
    CHECK(projection(3, 2) == -1.f);

    const Matrix4f move = Matrix4f::translation(1.f, 2.f, 3.f);
    CHECK(move.transformPoint(Vec3f(1.f, 1.f, 1.f)) == Vec3f(2.f, 3.f, 4.f));
    CHECK(move.transformDirection(Vec3f(1.f, 1.f, 1.f)) == Vec3f(1.f, 1.f, 1.f));
}
