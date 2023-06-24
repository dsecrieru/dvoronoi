//
// Created by Daniel Secrieru on 04/06/2023.
//

#ifndef DVORONOI_ARC_HPP
#define DVORONOI_ARC_HPP

#include <memory>

#include "dvoronoi/common/diagram.hpp"

namespace dvoronoi::fortune::_details {
    template<typename diag_t>
    struct arc_t {
        enum class color_t {Red, Black};
        enum class side_t {Left, Right};

        arc_t<diag_t>* parent = nullptr;
        arc_t<diag_t>* left = nullptr;
        arc_t<diag_t>* right = nullptr;

        arc_t<diag_t>* prev = nullptr;
        arc_t<diag_t>* next = nullptr;

        diag_t::site_t* site;
        diag_t::half_edge_t* left_half_edge = nullptr;
        diag_t::half_edge_t* right_half_edge = nullptr;

        std::weak_ptr<bool> is_event_valid{};

        color_t color = color_t::Black;
        side_t side = side_t::Left;
    };
}

#endif //DVORONOI_ARC_HPP
