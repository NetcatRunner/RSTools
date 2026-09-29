#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <ostream>
#include <type_traits>

namespace RST::Maths {

    /// 2D vector; arithmetic operators work component-wise, with a vector or a scalar.
    template<typename T>
    struct Vector2D {
        T x{}, y{};

        constexpr Vector2D() noexcept = default;
        constexpr Vector2D(T xValue, T yValue) noexcept : x(xValue), y(yValue) {}

        constexpr void set(const T& newX, const T& newY) noexcept { x = newX; y = newY; }
        [[nodiscard]] constexpr T getX() const noexcept { return x; }
        [[nodiscard]] constexpr T getY() const noexcept { return y; }

        [[nodiscard]] constexpr T& operator[](std::size_t n) noexcept {
            assert(n < 2 && "Vector2D index out of range");
            return n == 0 ? x : y;
        }
        [[nodiscard]] constexpr const T& operator[](std::size_t n) const noexcept {
            assert(n < 2 && "Vector2D index out of range");
            return n == 0 ? x : y;
        }
        [[nodiscard]] constexpr T* data() noexcept { return &x; }
        [[nodiscard]] constexpr const T* data() const noexcept { return &x; }

        [[nodiscard]] constexpr Vector2D operator+() const noexcept { return *this; }
        [[nodiscard]] constexpr Vector2D operator-() const noexcept { return Vector2D(-x, -y); }

        constexpr Vector2D& operator+=(const Vector2D& v) noexcept { x += v.x; y += v.y; return *this; }
        constexpr Vector2D& operator-=(const Vector2D& v) noexcept { x -= v.x; y -= v.y; return *this; }
        constexpr Vector2D& operator*=(const Vector2D& v) noexcept { x *= v.x; y *= v.y; return *this; }
        constexpr Vector2D& operator/=(const Vector2D& v) noexcept {
            if constexpr (std::is_integral_v<T>) {
                assert(v.x != 0 && v.y != 0 && "Vector2D integer division by zero");
            }
            x /= v.x; y /= v.y;
            return *this;
        }

        constexpr Vector2D& operator*=(T s) noexcept { x *= s; y *= s; return *this; }
        constexpr Vector2D& operator/=(T s) noexcept {
            if constexpr (std::is_integral_v<T>) {
                assert(s != 0 && "Vector2D integer division by zero");
            }
            x /= s; y /= s;
            return *this;
        }
        constexpr Vector2D& operator+=(T s) noexcept { x += s; y += s; return *this; }
        constexpr Vector2D& operator-=(T s) noexcept { x -= s; y -= s; return *this; }

        [[nodiscard]] constexpr bool operator==(const Vector2D&) const noexcept = default;

        [[nodiscard]] constexpr T dot(const Vector2D& v) const noexcept { return x * v.x + y * v.y; }
        /// Z component of the 3D cross product, positive when `v` is counterclockwise from this vector.
        [[nodiscard]] constexpr T cross(const Vector2D& v) const noexcept { return x * v.y - y * v.x; }
        /// This vector rotated by 90 degrees counterclockwise.
        [[nodiscard]] constexpr Vector2D perpendicular() const noexcept { return Vector2D(-y, x); }

        [[nodiscard]] constexpr auto lengthSquared() const noexcept { return x * x + y * y; }
        [[nodiscard]] constexpr auto length() const noexcept { return std::sqrt(lengthSquared()); }
        [[nodiscard]] constexpr auto distanceSquared(const Vector2D& v) const noexcept { return Vector2D(x - v.x, y - v.y).lengthSquared(); }
        [[nodiscard]] constexpr auto distance(const Vector2D& v) const noexcept { return Vector2D(x - v.x, y - v.y).length(); }

        /// Scales this vector to length 1; a zero vector is left unchanged.
        constexpr Vector2D& normalize() noexcept {
            const auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= static_cast<T>(len);
            return *this;
        }
        [[nodiscard]] constexpr Vector2D normalized() const noexcept { return Vector2D(*this).normalize(); }

        /// Converts the components, such as `v.to<int>()`.
        template<typename U>
        [[nodiscard]] constexpr Vector2D<U> to() const noexcept {
            return Vector2D<U>(static_cast<U>(x), static_cast<U>(y));
        }

        /// Component-wise minimum.
        [[nodiscard]] constexpr Vector2D min(const Vector2D& v) const noexcept {
            return Vector2D(v.x < x ? v.x : x, v.y < y ? v.y : y);
        }

        /// Component-wise maximum.
        [[nodiscard]] constexpr Vector2D max(const Vector2D& v) const noexcept {
            return Vector2D(x < v.x ? v.x : x, y < v.y ? v.y : y);
        }
    };

    /// @name Vector2D operators
    /// @{
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator+(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs += rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator-(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs -= rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator*(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs *= rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator/(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs /= rhs; }

    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator*(Vector2D<T> v, std::type_identity_t<T> s) noexcept { return v *= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator*(std::type_identity_t<T> s, Vector2D<T> v) noexcept { return v *= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator/(Vector2D<T> v, std::type_identity_t<T> s) noexcept { return v /= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator+(Vector2D<T> v, std::type_identity_t<T> s) noexcept { return v += s; }
    template<typename T>
    [[nodiscard]] constexpr Vector2D<T> operator-(Vector2D<T> v, std::type_identity_t<T> s) noexcept { return v -= s; }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2D<T>& vec) {
        return os << "(" << vec.getX() << ", " << vec.getY() << ")";
    }
    /// @}

    using Vec2f = Vector2D<float>;
    using Vec2d = Vector2D<double>;
    using Vec2i = Vector2D<int>;
    using Vec2u = Vector2D<unsigned int>;


    /// 3D vector; arithmetic operators work component-wise, with a vector or a scalar.
    template<typename T>
    struct Vector3D {
        T x{}, y{}, z{};

        constexpr Vector3D() noexcept = default;
        constexpr Vector3D(T xValue, T yValue, T zValue) noexcept : x(xValue), y(yValue), z(zValue) {}

        constexpr void set(const T& newX, const T& newY, const T& newZ) noexcept { x = newX; y = newY; z = newZ; }
        [[nodiscard]] constexpr T getX() const noexcept { return x; }
        [[nodiscard]] constexpr T getY() const noexcept { return y; }
        [[nodiscard]] constexpr T getZ() const noexcept { return z; }

        [[nodiscard]] constexpr T& operator[](std::size_t n) noexcept {
            assert(n < 3 && "Vector3D index out of range");
            return n == 0 ? x : (n == 1 ? y : z);
        }
        [[nodiscard]] constexpr const T& operator[](std::size_t n) const noexcept {
            assert(n < 3 && "Vector3D index out of range");
            return n == 0 ? x : (n == 1 ? y : z);
        }
        [[nodiscard]] constexpr T* data() noexcept { return &x; }
        [[nodiscard]] constexpr const T* data() const noexcept { return &x; }

        [[nodiscard]] constexpr Vector3D operator+() const noexcept { return *this; }
        [[nodiscard]] constexpr Vector3D operator-() const noexcept { return Vector3D(-x, -y, -z); }

        constexpr Vector3D& operator+=(const Vector3D& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
        constexpr Vector3D& operator-=(const Vector3D& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
        constexpr Vector3D& operator*=(const Vector3D& v) noexcept { x *= v.x; y *= v.y; z *= v.z; return *this; }
        constexpr Vector3D& operator/=(const Vector3D& v) noexcept {
            if constexpr (std::is_integral_v<T>) {
                assert(v.x != 0 && v.y != 0 && v.z != 0 && "Vector3D integer division by zero");
            }
            x /= v.x; y /= v.y; z /= v.z;
            return *this;
        }

        constexpr Vector3D& operator*=(T s) noexcept { x *= s; y *= s; z *= s; return *this; }
        constexpr Vector3D& operator/=(T s) noexcept {
            if constexpr (std::is_integral_v<T>) {
                assert(s != 0 && "Vector3D integer division by zero");
            }
            x /= s; y /= s; z /= s;
            return *this;
        }
        constexpr Vector3D& operator+=(T s) noexcept { x += s; y += s; z += s; return *this; }
        constexpr Vector3D& operator-=(T s) noexcept { x -= s; y -= s; z -= s; return *this; }

        [[nodiscard]] constexpr bool operator==(const Vector3D&) const noexcept = default;

        [[nodiscard]] constexpr T dot(const Vector3D& v) const noexcept { return x * v.x + y * v.y + z * v.z; }
        [[nodiscard]] constexpr Vector3D cross(const Vector3D& v) const noexcept {
            return Vector3D(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }

        [[nodiscard]] constexpr auto lengthSquared() const noexcept { return x * x + y * y + z * z; }
        [[nodiscard]] constexpr auto length() const noexcept { return std::sqrt(lengthSquared()); }
        [[nodiscard]] constexpr auto distanceSquared(const Vector3D& v) const noexcept { return Vector3D(x - v.x, y - v.y, z - v.z).lengthSquared(); }
        [[nodiscard]] constexpr auto distance(const Vector3D& v) const noexcept { return Vector3D(x - v.x, y - v.y, z - v.z).length(); }

        /// Scales this vector to length 1; a zero vector is left unchanged.
        constexpr Vector3D& normalize() noexcept {
            const auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= static_cast<T>(len);
            return *this;
        }
        [[nodiscard]] constexpr Vector3D normalized() const noexcept { return Vector3D(*this).normalize(); }

        /// Converts the components, such as `v.to<int>()`.
        template<typename U>
        [[nodiscard]] constexpr Vector3D<U> to() const noexcept {
            return Vector3D<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
        }

        /// Component-wise minimum.
        [[nodiscard]] constexpr Vector3D min(const Vector3D& v) const noexcept {
            return Vector3D(v.x < x ? v.x : x, v.y < y ? v.y : y, v.z < z ? v.z : z);
        }

        /// Component-wise maximum.
        [[nodiscard]] constexpr Vector3D max(const Vector3D& v) const noexcept {
            return Vector3D(x < v.x ? v.x : x, y < v.y ? v.y : y, z < v.z ? v.z : z);
        }
    };

    /// @name Vector3D operators
    /// @{
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator+(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs += rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator-(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs -= rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator*(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs *= rhs; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator/(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs /= rhs; }

    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator*(Vector3D<T> v, std::type_identity_t<T> s) noexcept { return v *= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator*(std::type_identity_t<T> s, Vector3D<T> v) noexcept { return v *= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator/(Vector3D<T> v, std::type_identity_t<T> s) noexcept { return v /= s; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator+(Vector3D<T> v, std::type_identity_t<T> s) noexcept { return v += s; }
    template<typename T>
    [[nodiscard]] constexpr Vector3D<T> operator-(Vector3D<T> v, std::type_identity_t<T> s) noexcept { return v -= s; }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3D<T>& vec) {
        return os << "(" << vec.getX() << ", " << vec.getY() << ", " << vec.getZ() << ")";
    }
    /// @}

    using Vec3f = Vector3D<float>;
    using Vec3d = Vector3D<double>;
    using Vec3i = Vector3D<int>;
    using Vec3u = Vector3D<unsigned int>;

}
