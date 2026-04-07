#pragma once

#include <iostream>

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
    
        T getX() const { return _x; };
        T getY() const { return _y; };
    
        void setX(T newX) { _x = newX; };
        void setY(T newY) { _y = newY; };
    
        Vector2D& Add(const Vector2D& vec) {
            _x += vec.getX();
            _y += vec.getY();
    
            return *this;
        };
        Vector2D& Substract(const Vector2D& vec) {
            _x -= vec.getX();
            _y -= vec.getY();
    
            return *this;
        };
        Vector2D& Multiply(const Vector2D& vec) {
            _x *= vec.getX();
            _y *= vec.getY();
    
            return *this;
        };
        Vector2D& Divide(const Vector2D& vec) {
            _x /= vec.getX();
            _y /= vec.getY();
    
            return *this;
        };
    
        Vector2D& operator+=(const Vector2D& vec) {return this->Add(vec);};
        Vector2D& operator-=(const Vector2D& vec) {return this->Substract(vec);};
        Vector2D& operator*=(const Vector2D& vec) {return this->Multiply(vec);};
        Vector2D& operator/=(const Vector2D& vec) {return this->Divide(vec);};
    };
    
    template<typename T>
    Vector2D<T>& operator+(Vector2D<T>& v1, const Vector2D<T>& v2) {return v1.Add(v2);};
    
    template<typename T>
    Vector2D<T>& operator-(Vector2D<T>& v1, const Vector2D<T>& v2) {return v1.Substract(v2);};
    
    template<typename T>
    Vector2D<T>& operator*(Vector2D<T>& v1, const Vector2D<T>& v2) {return v1.Multiply(v2);};
    
    template<typename T>
    Vector2D<T>& operator/(Vector2D<T>& v1, const Vector2D<T>& v2) {return v1.Divide(v2);};
    
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2D<T>& vec) {
        os << "(" << vec.getX() << ", " << vec.getY() << ")";
        return os;
    }
    
    using Vec2f = Vector2D<float>;
    using Vec2i = Vector2D<int>;
}
