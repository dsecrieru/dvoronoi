//
// Created by Daniel Secrieru on 24/07/2023.
//

#ifndef DVORONOI_BOX_HPP
#define DVORONOI_BOX_HPP

#include "scalar.hpp"

namespace dvoronoi {

    namespace box_side {
        constexpr std::size_t Left = 0;
        constexpr std::size_t Bottom = 1;
        constexpr std::size_t Right = 2;
        constexpr std::size_t Top = 3;
    }

    struct box_t {
        _internal::scalar_t left = -std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t bottom = -std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t right = std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t top = std::numeric_limits<_internal::scalar_t>::infinity();
    };

} // namespace dvoronoi::_internal

#endif //DVORONOI_BOX_HPP
