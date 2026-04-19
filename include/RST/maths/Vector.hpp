#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace RST::Maths {

    template<typename T>
    class Vector2D
    {
    private:
        T _x{};
        T _y{};
    public:
        Vector2D() = default;
        Vector2D(T x, T y): _x(x), _y(y) {};
    
        ~Vector2D() = default;

        constexpr void set(const T& x, const T& y) {_x = x; _y = y;};
        constexpr T getX() const noexcept {return _x;};
        constexpr T getY() const noexcept {return _y;};
        constexpr T& operator[](const std::size_t n) {
            switch (n) {
                case 0: return _x;
                case 1: return _y;
                default: throw std::out_of_range("Array index out of bounds");
            }
        };

        constexpr Vector2D operator+() const {
            return *this;
        }
        constexpr Vector2D operator-() const {
            return Vector2D(-_x, -_y);
        }
    
        constexpr Vector2D& operator+=(const Vector2D& v) noexcept { _x += v._x; _y += v._y; return *this; };
        constexpr Vector2D& operator-=(const Vector2D& v) noexcept { _x -= v._x; _y -= v._y; return *this; };
        constexpr Vector2D& operator*=(const Vector2D& v) noexcept { _x *= v._x; _y *= v._y; return *this; };
        constexpr Vector2D& operator/=(const Vector2D& v) {
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(v._x) < std::numeric_limits<T>::epsilon() || std::abs(v._y) < std::numeric_limits<T>::epsilon())
                    throw std::invalid_argument("Division by zero");
            } else {
                if (v._x == 0 || v._y == 0)
                    throw std::invalid_argument("Division by zero");
            }
            _x /= v._x; _y /= v._y;
            return *this;
        };

        constexpr Vector2D& operator*=(T s) {
            _x *= s;
            _y *= s;
            return *this;
        };
        constexpr Vector2D& operator/=(T s) {
            if (s == 0)
                throw std::invalid_argument("Division by zero");
            _x /= s;
            _y /= s;
            return *this;
        };
        constexpr Vector2D& operator+=(T s) {
            _x += s;
            _y += s;
            return *this;
        };
        constexpr Vector2D& operator-=(T s) {
            _x -= s;
            _y -= s;
            return *this;
        };

        constexpr T dot(const Vector2D& v) const { return _x * v._x + _y * v._y; };

        constexpr auto length() const {return std::sqrt(_x *_x + _y * _y);};
        constexpr Vector2D& normalize() {
            auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= len;
            return *this;
        }
        constexpr Vector2D normalized() const { return Vector2D(*this).normalize(); };

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
        T _x{}, _y{}, _z{};

        constexpr Vector3D() noexcept = default;
        constexpr Vector3D(T x, T y, T z) noexcept : _x(x), _y(y), _z(z) {}

        constexpr void set(const T& x, const T& y, const T& z) {_x = x; _y = y; _z = z;};
        constexpr T getX() const noexcept {return _x;};
        constexpr T getY() const noexcept {return _y;};
        constexpr T getZ() const noexcept {return _z;};
        constexpr T& operator[](const std::size_t n) {
            switch (n) {
                case 0: return _x;
                case 1: return _y;
                case 2: return _z;
                default: throw std::out_of_range("Array index out of bounds");
            }
        };

        constexpr Vector3D operator+() const {
            return *this;
        }

        constexpr Vector3D operator-() const noexcept {
            return Vector3D(-_x, -_y, -_z);
        };

        constexpr Vector3D& operator+=(const Vector3D& v) noexcept { _x += v._x; _y += v._y; _z += v._z; return *this; };
        constexpr Vector3D& operator-=(const Vector3D& v) noexcept { _x -= v._x; _y -= v._y; _z -= v._z; return *this; };
        constexpr Vector3D& operator*=(const Vector3D& v) noexcept { _x *= v._x; _y *= v._y; _z *= v._z; return *this; };
        constexpr Vector3D& operator/=(const Vector3D& v) {
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(v._x) < std::numeric_limits<T>::epsilon() || std::abs(v._y) < std::numeric_limits<T>::epsilon() || std::abs(v._z) < std::numeric_limits<T>::epsilon())
                    throw std::invalid_argument("Division by zero");
            } else {
                if (v._x == 0 || v._y == 0 || v._z == 0)
                    throw std::invalid_argument("Division by zero");
            }
            _x /= v._x; _y /= v._y; _z /= v._z;
            return *this;
        };

        constexpr Vector3D& operator*=(T s) noexcept {
            _x *= s;
            _y *= s;
            _z *= s;
            return *this;
        };
        constexpr Vector3D& operator/=(T s) { 
            if (s == 0)
                throw std::invalid_argument("Division by zero");
            _x /= s;
            _y /= s;
            _z /= s;
            return *this;
        };
        constexpr Vector3D& operator+=(T s) noexcept {
            _x += s;
            _y += s;
            _z += s;
            return *this;
        };
        constexpr Vector3D& operator-=(T s) noexcept {
            _x -= s;
            _y -= s;
            _z -= s;
            return *this; 
        };

        constexpr T dot(const Vector3D& v) const noexcept { return _x * v._x + _y * v._y + _z * v._z; };

        constexpr auto length() const {return std::sqrt(_x *_x + _y * _y + _z * _z);};
        constexpr Vector3D& normalize() {
            auto len = length();
            if (len > std::numeric_limits<T>::epsilon())
                *this /= len;
            return *this;
        }
        constexpr Vector3D normalized() const { return Vector2D(*this).normalize(); };
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
