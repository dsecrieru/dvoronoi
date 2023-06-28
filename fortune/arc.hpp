//
// Created by Daniel Secrieru on 04/06/2023.
//

#ifndef DVORONOI_ARC_HPP
#define DVORONOI_ARC_HPP

#include "event.hpp"

namespace dvoronoi::fortune::_details {

    template<typename diag_traits>
    struct event_t;

    template<typename diag_traits>
    struct arc_t {
        enum class color_t { Red, Black };
        enum class side_t { Left, Right };

        arc_t* parent = nullptr;
        arc_t* left = nullptr;
        arc_t* right = nullptr;

        arc_t* prev = nullptr;
        arc_t* next = nullptr;

        diag_traits::site_t* site;
        diag_traits::half_edge_t* left_half_edge = nullptr;
        diag_traits::half_edge_t* right_half_edge = nullptr;

        event_t<diag_traits>* event = nullptr;

        color_t color = color_t::Black;
        side_t side = side_t::Left;
    };
}

#endif //DVORONOI_ARC_HPP
