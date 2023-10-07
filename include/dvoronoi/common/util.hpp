//
// Created by Daniel Secrieru on 16/05/2023.
//

#ifndef DVORONOI_UTIL_HPP
#define DVORONOI_UTIL_HPP

namespace dvoronoi::util {

    template<typename T>
    bool eq(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return std::fabs(v2 - v1) < eps;
    }

    template<typename T>
    bool is_zero(T val) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return std::fabs(val) < eps;
    }

    template<typename T>
    bool lt(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 < v2 - eps;
    }

    template<typename T>
    bool lt_eq(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 <= v2 + eps;
    }

    template<typename T>
    bool gt(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 > v2 + eps;
    }

    template<typename T>
    bool gt_eq(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 >= v2 - eps;
    }

    template<typename T>
    bool between(T x, T v1, T v2) {
        return gt(x, v1) && lt(x, v2);
    }

    template<typename T>
    bool between_eq(T x, T v1, T v2) {
        return gt_eq(x, v1) && lt_eq(x, v2);
    }

} // namespace dvoronoi::util

#endif //DVORONOI_UTIL_HPP
