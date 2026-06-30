#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <algorithm>

namespace RST::Maths {

    template<typename T>
    struct Vector2D {
        T x{}, y{};

        constexpr Vector2D() noexcept = default;
        constexpr Vector2D(T x, T y) noexcept : x(x), y(y) {}
    
        ~Vector2D() = default;

        constexpr void set(const T& x, const T& y) {x = x; y = y;};
        constexpr T getX() const noexcept {return x;};
        constexpr T getY() const noexcept {return y;};
        constexpr T& operator[](const std::size_t n) {
            switch (n) {
                case 0: return x;
                case 1: return y;
                default: throw std::out_of_range("Array index out of bounds");
            }
        };
        constexpr T* data() noexcept { return &x; };
        constexpr const T* data() const noexcept { return &x; };

        constexpr Vector2D operator+() const {
            return *this;
        }
        constexpr Vector2D operator-() const {
            return Vector2D(-x, -y);
        }
    
        constexpr Vector2D& operator+=(const Vector2D& v) noexcept { x += v.x; y += v.y; return *this; };
        constexpr Vector2D& operator-=(const Vector2D& v) noexcept { x -= v.x; y -= v.y; return *this; };
        constexpr Vector2D& operator*=(const Vector2D& v) noexcept { x *= v.x; y *= v.y; return *this; };
        constexpr Vector2D& operator/=(const Vector2D& v) {
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(v.x) < std::numeric_limits<T>::epsilon() || std::abs(v.y) < std::numeric_limits<T>::epsilon())
                    throw std::invalid_argument("Division by zero");
            } else {
                if (v.x == 0 || v.y == 0)
                    throw std::invalid_argument("Division by zero");
            }
            x /= v.x; y /= v.y;
            return *this;
        };

        constexpr Vector2D& operator*=(T s) {
            x *= s;
            y *= s;
            return *this;
        };
        constexpr Vector2D& operator/=(T s) {
            if (s == 0)
                throw std::invalid_argument("Division by zero");
            x /= s;
            y /= s;
            return *this;
        };
        constexpr Vector2D& operator+=(T s) {
            x += s;
            y += s;
            return *this;
        };
        constexpr Vector2D& operator-=(T s) {
            x -= s;
            y -= s;
            return *this;
        };

        constexpr T dot(const Vector2D& v) const { return x * v.x + y * v.y; };

        constexpr auto length_sqrt() const noexcept { return (x * x + y * y); }
        constexpr auto length() const {return std::sqrt(x *x + y * y);};
        constexpr Vector2D& normalize() {
            auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= len;
            return *this;
        }
        constexpr Vector2D normalized() const { return Vector2D(*this).normalize(); };

        template<typename U>
        constexpr Vector2D<U> to() const noexcept {
            return Vector2D<U>(static_cast<U>(x), static_cast<U>(y));
        }

        constexpr Vector2D min(const Vector2D& v) const noexcept {
            return Vector2D(std::min(x, v.x), std::min(y, v.y));
        }

        constexpr Vector2D max(const Vector2D& v) const noexcept {
            return Vector2D(std::max(x, v.x), std::max(y, v.y));
        }
    };
    
    template<typename T>
    constexpr Vector2D<T> operator+(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs += rhs; };
    template<typename T>
    constexpr Vector2D<T> operator-(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs -= rhs; };
    template<typename T>
    constexpr Vector2D<T> operator*(Vector2D<T> lhs, const Vector2D<T>& rhs) noexcept { return lhs *= rhs; };
    template<typename T>
    constexpr Vector2D<T> operator/(Vector2D<T> lhs, const Vector2D<T>& rhs) { return lhs /= rhs; };

    template<typename T>
    constexpr Vector2D<T> operator*(Vector2D<T> v, T s) noexcept { return v *= s; };
    template<typename T>
    constexpr Vector2D<T> operator/(Vector2D<T> v, T s) { return v /= s; };
    template<typename T>
    constexpr Vector2D<T> operator+(Vector2D<T> v, T s) noexcept { return v += s; };
    template<typename T>
    constexpr Vector2D<T> operator-(Vector2D<T> v, T s) noexcept { return v -= s; };
    
    template<typename T>
    constexpr bool operator==(const Vector2D<T>& lhs, const Vector2D<T>& rhs) noexcept {
        return (lhs.getX() == rhs.getX() && lhs.getY() == rhs.getY());
    }
    template<typename T>
    constexpr bool operator!=(const Vector2D<T>& lhs, const Vector2D<T>& rhs) noexcept { return !(lhs == rhs); }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2D<T>& vec) {
        return os << "(" << vec.getX() << ", " << vec.getY() << ")";
    }
    
    using Vec2f = Vector2D<float>;
    using Vec2i = Vector2D<int>;


    template<typename T>
    struct Vector3D {
        T x{}, y{}, z{};

        constexpr Vector3D() noexcept = default;
        constexpr Vector3D(T x, T y, T z) noexcept : x(x), y(y), z(z) {}

        constexpr void set(const T& x, const T& y, const T& z) {x = x; y = y; z = z;};
        constexpr T getX() const noexcept {return x;};
        constexpr T getY() const noexcept {return y;};
        constexpr T getZ() const noexcept {return z;};
        constexpr T operator[](const std::size_t n) const {
            switch (n) {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: throw std::out_of_range("Array index out of bounds");
            }
        };
        constexpr T* data() noexcept { return &x; };
        constexpr const T* data() const noexcept { return &x; };

        constexpr Vector3D operator+() const {
            return *this;
        }

        constexpr Vector3D operator-() const noexcept {
            return Vector3D(-x, -y, -z);
        };

        constexpr Vector3D& operator+=(const Vector3D& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; };
        constexpr Vector3D& operator-=(const Vector3D& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; };
        constexpr Vector3D& operator*=(const Vector3D& v) noexcept { x *= v.x; y *= v.y; z *= v.z; return *this; };
        constexpr Vector3D& operator/=(const Vector3D& v) {
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(v.x) < std::numeric_limits<T>::epsilon() || std::abs(v.y) < std::numeric_limits<T>::epsilon() || std::abs(v.z) < std::numeric_limits<T>::epsilon())
                    throw std::invalid_argument("Division by zero");
            } else {
                if (v.x == 0 || v.y == 0 || v.z == 0)
                    throw std::invalid_argument("Division by zero");
            }
            x /= v.x; y /= v.y; z /= v.z;
            return *this;
        };

        constexpr Vector3D& operator*=(T s) noexcept {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        };
        constexpr Vector3D& operator/=(T s) { 
            if (s == 0)
                throw std::invalid_argument("Division by zero");
            x /= s;
            y /= s;
            z /= s;
            return *this;
        };
        constexpr Vector3D& operator+=(T s) noexcept {
            x += s;
            y += s;
            z += s;
            return *this;
        };
        constexpr Vector3D& operator-=(T s) noexcept {
            x -= s;
            y -= s;
            z -= s;
            return *this; 
        };

        constexpr T dot(const Vector3D& v) const noexcept { return x * v.x + y * v.y + z * v.z; };
        constexpr Vector3D cross(const Vector3D& v) const noexcept {
            return Vector3D(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        };

        constexpr auto length_sqrt() const noexcept { return (x * x + y * y + z * z); }
        inline auto length() const noexcept { return std::sqrt(length_sqrt()); }
        inline Vector3D& normalize() {
            auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= len;
            return *this;
        }        
        inline Vector3D normalized() const noexcept { return Vector3D(*this).normalize(); }

        template<typename U>
        constexpr Vector3D<U> to() const noexcept {
            return Vector3D<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
        }

        constexpr Vector3D min(const Vector3D& v) const noexcept {
            return Vector3D(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z));
        }

        constexpr Vector3D max(const Vector3D& v) const noexcept {
            return Vector3D(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z));
        }
    };

    template<typename T>
    constexpr Vector3D<T> operator+(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs += rhs; };
    template<typename T>
    constexpr Vector3D<T> operator-(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs -= rhs; };
    template<typename T>
    constexpr Vector3D<T> operator*(Vector3D<T> lhs, const Vector3D<T>& rhs) noexcept { return lhs *= rhs; };
    template<typename T>
    constexpr Vector3D<T> operator/(Vector3D<T> lhs, const Vector3D<T>& rhs) { return lhs /= rhs; };

    template<typename T>
    constexpr Vector3D<T> operator*(Vector3D<T> v, T s) noexcept { return v *= s; };
    template<typename T>
    constexpr Vector3D<T> operator*(T s, Vector3D<T> v) noexcept { return v *= s; };
    template<typename T>
    constexpr Vector3D<T> operator/(Vector3D<T> v, T s) { return v /= s; };
    template<typename T>
    constexpr Vector3D<T> operator+(Vector3D<T> v, T s) noexcept { return v += s; };
    template<typename T>
    constexpr Vector3D<T> operator-(Vector3D<T> v, T s) noexcept { return v -= s; };

    template<typename T>
    constexpr bool operator==(const Vector3D<T>& lhs, const Vector3D<T>& rhs) noexcept {
        return (lhs.getX()== rhs.getX() && lhs.getY()== rhs.getY() && lhs.getZ() == rhs.getZ());
    }

    template<typename T>
    constexpr bool operator!=(const Vector3D<T>& lhs, const Vector3D<T>& rhs) noexcept { return !(lhs == rhs); }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3D<T>& vec) {
        return os << "(" << vec.getX() << ", " << vec.getY() << ", " << vec.getZ() << ")";
    }

    using Vec3f = Vector3D<float>;
    using Vec3i = Vector3D<int>;
    
}
