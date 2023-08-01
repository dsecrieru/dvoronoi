//
// Created by Daniel Secrieru on 04/06/2023.
//

#ifndef DVORONOI_ARC_HPP
#define DVORONOI_ARC_HPP

#include "event.hpp"

namespace dvoronoi::fortune::_details {

    template<typename diag_traits>
    struct event_t;

    namespace data {
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
    } // namespace data

    void set_dest(auto* left, auto* right, auto* vertex) {
        left->right_half_edge->orig = vertex;
        right->left_half_edge->dest = vertex;
    }

    void set_orig(auto* left, auto* right, auto* vertex) {
        left->right_half_edge->dest = vertex;
        right->left_half_edge->orig = vertex;
    }

    void set_prev_half_edge(auto* prev, auto* next) {
        prev->next = next;
        next->prev = prev;
    }

}

#endif //DVORONOI_ARC_HPP
