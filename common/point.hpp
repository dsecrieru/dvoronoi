//
// Created by Daniel Secrieru on 01/06/2023.
//

#ifndef DVORONOI_POINT_HPP
#define DVORONOI_POINT_HPP

#include <cmath>

namespace dvoronoi::_internal {

    typedef double scalar_t;

    struct point2_t;
    point2_t operator-(point2_t lhs, const point2_t& rhs);

    struct point2_t {
        scalar_t x{};
        scalar_t y{};

        point2_t& operator+=(const point2_t& p) { x += p.x; y += p.y; return *this; }
        point2_t& operator-=(const point2_t& p) { x -= p.x; y -= p.y; return *this; }
        point2_t& operator*=(const scalar_t s) { x *= s; y *= s; return *this; }

        [[nodiscard]] scalar_t norm() const { return std::sqrt(x * x + y * y); };
        [[nodiscard]] point2_t ortho() const { return { -y, x }; }
        [[nodiscard]] scalar_t det(const point2_t& p) const { return x * p.y - y * p.x; }
        [[nodiscard]] scalar_t dist(const point2_t& p) const { return (*this - p).norm(); }
    };

    point2_t operator+(point2_t lhs, const point2_t& rhs) {
        lhs += rhs;
        return lhs;
    }

    point2_t operator-(point2_t lhs, const point2_t& rhs) {
        lhs -= rhs;
        return lhs;
    }

    template<typename T>
    point2_t operator*(T scalar, point2_t p) {
        p *= scalar;
        return p;
    }

    template<typename T>
    point2_t operator*(point2_t p, T scalar) { return scalar * p; }
}

#endif //DVORONOI_POINT_HPP
