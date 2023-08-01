//
// Created by Daniel Secrieru on 01/08/2023.
//

#ifndef EMERGENT_DATA_HPP
#define EMERGENT_DATA_HPP

#include "point.hpp"

namespace dvoronoi::data {
    typedef _internal::scalar_t scalar_t;
    typedef _internal::point2_t point_t;

    struct face_t;

    struct site_t {
        std::size_t index;
        point_t point{};
        face_t* face = nullptr;

        explicit site_t(std::size_t i, scalar_t x, scalar_t y) : index(i), point(x, y) {}
    };

    struct vertex_t {
        point_t point{};
    };

    struct half_edge_t {
        vertex_t* orig = nullptr;
        vertex_t* dest = nullptr;
        half_edge_t* twin = nullptr;
        face_t* face = nullptr;

        half_edge_t* prev = nullptr;
        half_edge_t* next = nullptr;
    };

    struct face_t {
        site_t* site = nullptr;
        half_edge_t* half_edge = nullptr;
    };
}

#endif //EMERGENT_DATA_HPP
