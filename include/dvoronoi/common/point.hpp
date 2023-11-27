//
// Created by Daniel Secrieru on 01/06/2023.
//

#ifndef DVORONOI_POINT_HPP
#define DVORONOI_POINT_HPP

#include <cmath>

#include "scalar.hpp"
#include "util.hpp"

namespace dvoronoi::_internal {

    struct point2_t;
    static point2_t operator-(point2_t lhs, const point2_t& rhs);

    struct point2_t {
        scalar_t x{};
        scalar_t y{};

        point2_t& operator+=(const point2_t& p) { x += p.x; y += p.y; return *this; }
        point2_t& operator-=(const point2_t& p) { x -= p.x; y -= p.y; return *this; }
        point2_t& operator*=(const scalar_t s) { x *= s; y *= s; return *this; }
        point2_t& operator/=(const scalar_t s) { x /= s; y /= s; return *this; }

        [[nodiscard]] scalar_t norm() const { return std::sqrt(x * x + y * y); };
        [[nodiscard]] point2_t ortho() const { return { -y, x }; }
        [[nodiscard]] scalar_t det(const point2_t& p) const { return x * p.y - y * p.x; }
        [[nodiscard]] scalar_t dist(const point2_t& p) const { return (*this - p).norm(); }
        [[nodiscard]] scalar_t cross(const point2_t& p1, const point2_t& p2) const { return (p1.x - x) * (p2.y - y) - (p1.y - y) * (p2.x - x); }

        [[nodiscard]] bool is_between(const point2_t& p1, const point2_t& p2) const {
            return util::between_eq(x, std::min(p1.x, p2.x), std::max(p1.x, p2.x)) &&
                   util::between_eq(y, std::min(p1.y, p2.y), std::max(p1.y, p2.y));
        }
    };

    static point2_t operator+(point2_t lhs, const point2_t& rhs) {
        lhs += rhs;
        return lhs;
    }

    static point2_t operator-(point2_t lhs, const point2_t& rhs) {
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

    template<typename T>
    point2_t operator/(point2_t p, T scalar) {
        p /= scalar;
        return p;
    }
}

#endif //DVORONOI_POINT_HPP
