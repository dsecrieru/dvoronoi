//
// Created by Daniel Secrieru on 16/05/2023.
//

#ifndef DVORONOI_UTIL_HPP
#define DVORONOI_UTIL_HPP

#include <cmath>

namespace dvoronoi::util {

    template<typename T>
    bool eq(T v1, T v2) {
        return std::fabs(v2 - v1) < std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool is_zero(T val) {
        return std::fabs(val) < std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool lt(T v1, T v2) {
        return v1 < v2 - std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool gt(T v1, T v2) {
        return v1 > v2 + std::numeric_limits<T>::epsilon();
    }

    template<typename point2d_t>
    point2d_t add(const point2d_t& lhs, const point2d_t& rhs) {
        return point2d_t(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    template<typename point2d_t>
    point2d_t diff(const point2d_t& lhs, const point2d_t& rhs) {
        return point2d_t(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    template<typename T, typename point2d_t>
    point2d_t mult(T scalar, const point2d_t& p) {
        return point2d_t(scalar * p.x, scalar * p.y);
    }

    template<typename point2d_t>
    auto norm(const point2d_t& p) {
        return std::sqrt(p.x * p.x + p.y * p.y);
    }

    template<typename point2d_t>
    point2d_t ortho(const point2d_t& p) {
        return point2d_t(-p.y, p.x);
    }

    template<typename point2d_t>
    auto determinant(const point2d_t& p1, const point2d_t& p2) {
        return p1.x * p2.y - p1.y * p2.x;
    }

    template<typename point2d_t>
    auto distance(const point2d_t& p1, const point2d_t& p2) {
        return norm(diff(p1, p2));
    }

} // namespace dvoronoi::util

#endif //DVORONOI_UTIL_HPP
