#pragma once

#include <cmath>
#include <array>
#include <stdexcept>
#include <initializer_list>
#include <iostream>
#include <algorithm>

#include "Vector.hpp"

namespace RST::Maths {
    template <typename T, size_t Rows, size_t Cols>
    class Matrix {
    public:
        constexpr Matrix() noexcept {
            for (size_t i = 0; i < Rows * Cols; ++i) {
                m[i] = 0;
            }
            size_t min_dim = (Rows < Cols) ? Rows : Cols;
            for (size_t i = 0; i < min_dim; i++) {
                (*this)(i, i) = 1;
            }
        }

        constexpr Matrix(T value) noexcept {
            for (size_t i = 0; i < Rows * Cols; ++i) m[i] = value;
        }

        constexpr Matrix(std::initializer_list<T> list) {
            if (list.size() != Rows * Cols) {
                throw std::invalid_argument("Invalid initializer list size for Matrix");
            }
            size_t i = 0;
            for (auto val : list)
                m[i++] = val;
        }

        constexpr T& operator[](size_t index) noexcept { return m[index]; }
        constexpr const T& operator[](size_t index) const noexcept { return m[index]; }
        constexpr T& operator()(size_t row, size_t col) noexcept { return m[row * Cols + col]; }
        constexpr const T& operator()(size_t row, size_t col) const noexcept { return m[row * Cols + col]; }

        constexpr Matrix<T, Rows, Cols> operator+(const Matrix<T, Rows, Cols>& other) const noexcept {
            Matrix<T, Rows, Cols> res(0);
            for (size_t i = 0; i < Rows * Cols; ++i) {
                res.m[i] = m[i] + other.m[i];
            }
            return res;
        }

        constexpr Matrix<T, Rows, Cols> operator-(const Matrix<T, Rows, Cols>& other) const noexcept {
            Matrix<T, Rows, Cols> res(0);
            for (size_t i = 0; i < Rows * Cols; ++i) {
                res.m[i] = m[i] - other.m[i];
            }
            return res;
        }

        template <size_t P>
        constexpr Matrix<T, Rows, P> operator*(const Matrix<T, Cols, P>& other) const noexcept {
            Matrix<T, Rows, P> res(0);
            for (size_t i = 0; i < Rows; ++i) {
                for (size_t j = 0; j < P; ++j) {
                    T sum = 0;
                    for (size_t k = 0; k < Cols; ++k) {
                        sum += (*this)(i, k) * other(k, j);
                    }
                    res(i, j) = sum;
                }
            }
            return res;
        }

        constexpr Matrix<T, Rows, Cols> operator*(T scalar) const noexcept {
            Matrix<T, Rows, Cols> res(0);
            for (size_t i = 0; i < Rows * Cols; ++i) {
                res.m[i] = m[i] * scalar;
            }
            return res;
        }

        constexpr Vector3D<T> transformPoint(const Vector3D<T>& p) const {
            static_assert(Rows >= 3 && Cols >= 4, "Matrix must be at least 3x4 to transform a 3D point");
            T res[3];
            for (size_t i = 0; i < 3; ++i) {
                res[i] = (*this)(i, 0) * p.getX() + (*this)(i, 1) * p.getY() + (*this)(i, 2) * p.getZ();
                res[i] += (*this)(i, 3);
            }
            return Vector3D<T>{res[0], res[1], res[2]};
        }

        constexpr Vector3D<T> transformDirection(const Vector3D<T>& d) const {
            static_assert(Rows >= 3 && Cols >= 3, "Matrix must be at least 3x3 to transform a 3D direction");
            T res[3];
            for (size_t i = 0; i < 3; ++i) {
                res[i] = (*this)(i, 0) * d.getX() + (*this)(i, 1) * d.getY() + (*this)(i, 2) * d.getZ();
            }
            return Vector3D<T>{res[0], res[1], res[2]};
        }

        constexpr Matrix<T, Cols, Rows> transpose() const noexcept {
            Matrix<T, Cols, Rows> res(0);
            for (size_t i = 0; i < Rows; ++i) {
                for (size_t j = 0; j < Cols; ++j) {
                    res(j, i) = (*this)(i, j);
                }
            }
            return res;
        }

        constexpr Matrix<T, Rows, Cols> inverse() const {
            static_assert(Rows == Cols, "Only square matrices can be inverted");
            Matrix<T, Rows, Cols> temp = *this; 
            Matrix<T, Rows, Cols> inv;

            for (size_t i = 0; i < Rows; ++i) {
                size_t pivot = i;
                for (size_t p = i + 1; p < Rows; ++p) {
                    if (std::abs(temp(p, i)) > std::abs(temp(pivot, i))) pivot = p;
                }

                if (std::abs(temp(pivot, i)) < std::numeric_limits<T>::epsilon()) {
                    throw std::runtime_error("Matrix is singular and cannot be inverted");
                }

                if (pivot != i) {
                    for (size_t c = 0; c < Cols; ++c) {
                        std::swap(temp(i, c), temp(pivot, c));
                        std::swap(inv(i, c), inv(pivot, c));
                    }
                }

                T factor = temp(i, i);
                for (size_t c = 0; c < Cols; ++c) {
                    temp(i, c) /= factor;
                    inv(i, c) /= factor;
                }

                for (size_t j = 0; j < Rows; ++j) {
                    if (i != j) {
                        T f = temp(j, i);
                        for (size_t c = 0; c < Cols; ++c) {
                            temp(j, c) -= f * temp(i, c);
                            inv(j, c) -= f * inv(i, c);
                        }
                    }
                }
            }
            return inv;
        }

        static constexpr Matrix<T, Rows, Cols> translation(T x, T y, T z) noexcept {
            static_assert(Rows >= 4 && Cols >= 4, "Matrix must be 4x4 or larger for 3D translation");
            Matrix<T, Rows, Cols> res;
            res(0, 3) = x;
            res(1, 3) = y;
            res(2, 3) = z;
            return res;
        }

        static constexpr Matrix<T, Rows, Cols> translation(const Vector3D<T>& v) noexcept {
            return translation(v.getX(), v.getY(), v.getZ());
        }

        static constexpr Matrix<T, Rows, Cols> scale(T sx, T sy, T sz) noexcept {
            static_assert(Rows >= 3 && Cols >= 3, "Matrix must be 3x3 or larger for 3D scaling");
            Matrix<T, Rows, Cols> res;
            res(0, 0) = sx;
            res(1, 1) = sy;
            res(2, 2) = sz;
            return res;
        }

        static constexpr Matrix<T, Rows, Cols> scale(T s) noexcept {
            return scale(s, s, s);
        }
        static constexpr Matrix<T, Rows, Cols> shear(T sxy, T sxz, T syx, T syz, T szx, T szy) noexcept {
            static_assert(Rows >= 3 && Cols >= 3, "Matrix must be 3x3 or larger for 3D shearing");
            Matrix<T, Rows, Cols> res;
            res(0, 1) = sxy;
            res(0, 2) = sxz;
            res(1, 0) = syx;
            res(1, 2) = syz;
            res(2, 0) = szx;
            res(2, 1) = szy;
            return res;
        }

        static Matrix<T, Rows, Cols> rotationX(T angleRad) {
            static_assert(Rows >= 3 && Cols >= 3, "Matrix must be 3x3 or larger for 3D rotation");
            Matrix<T, Rows, Cols> res;
            T c = std::cos(angleRad);
            T s = std::sin(angleRad);
            res(1, 1) = c;
            res(1, 2) = -s;
            res(2, 1) = s;
            res(2, 2) = c;
            return res;
        }

        static Matrix<T, Rows, Cols> rotationY(T angleRad) {
           static_assert(Rows >= 3 && Cols >= 3, "Matrix must be 3x3 or larger for 3D rotation");
            Matrix<T, Rows, Cols> res;
            T c = std::cos(angleRad);
            T s = std::sin(angleRad);
            
            res(0, 0) =  c;
            res(0, 2) =  s;
            res(2, 0) = -s;
            res(2, 2) =  c;
            
            return res;
        }

        static Matrix<T, Rows, Cols> rotationZ(T angleRad) {
            static_assert(Rows >= 3 && Cols >= 3, "Matrix must be 3x3 or larger for 3D rotation");
            Matrix<T, Rows, Cols> res;
            T c = std::cos(angleRad);
            T s = std::sin(angleRad);
            
            res(0, 0) =  c;
            res(0, 1) = -s;
            res(1, 0) =  s;
            res(1, 1) =  c;
            
            return res;
        }

        static constexpr Matrix<T, Rows, Cols> identity() noexcept {
            return Matrix<T, Rows, Cols>();
        }

        static Matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T nearP = T(-1), T farP = T(1)) {
            static_assert(Rows == 4 && Cols == 4, "ortho requires a 4x4 matrix");
            Matrix<T, 4, 4> r(T(0));
            r(0, 0) =  T(2) / (right - left);
            r(1, 1) =  T(2) / (top - bottom);
            r(2, 2) = -T(2) / (farP - nearP);
            r(0, 3) = -(right + left) / (right - left);
            r(1, 3) = -(top + bottom) / (top - bottom);
            r(2, 3) = -(farP + nearP) / (farP - nearP);
            r(3, 3) =  T(1);
            return r;
        }

    private:
        T m[Rows * Cols];
    };

    template <typename T> using Matrix4 = Matrix<T, 4, 4>;
    template <typename T> using Matrix3 = Matrix<T, 3, 3>;

    using Matrix4f = Matrix<float, 4, 4>;
    using Matrix3f = Matrix<float, 3, 3>;
    using Vector4f = Matrix<float, 4, 1>;
    using Vector3f = Matrix<float, 3, 1>;

    template <typename T, size_t Rows, size_t Cols>
    std::ostream& operator<<(std::ostream& os, const Matrix<T, Rows, Cols>& mat) {
        for (size_t i = 0; i < Rows; ++i) {
            os << "[ ";
            for (size_t j = 0; j < Cols; ++j) {
                os << mat(i, j) << (j < Cols - 1 ? ", " : " ");
            }
            os << "]\n";
        }
        return os;
    }
}
