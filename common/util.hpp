//
// Created by Daniel Secrieru on 16/05/2023.
//

#ifndef DVORONOI_UTIL_HPP
#define DVORONOI_UTIL_HPP

namespace dvoronoi::util {

    template<typename T>
    bool eq(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return std::fabs(v2 - v1) < std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool is_zero(T val) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return std::fabs(val) < std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool lt(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 < v2 - std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    bool gt(T v1, T v2) {
        constexpr auto eps = std::numeric_limits<T>::epsilon();
        return v1 > v2 + std::numeric_limits<T>::epsilon();
    }

} // namespace dvoronoi::util

#endif //DVORONOI_UTIL_HPP
