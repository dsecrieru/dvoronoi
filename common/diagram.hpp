//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_COMMON_DIAGRAM_HPP
#define DVORONOI_COMMON_DIAGRAM_HPP

#include <vector>
#include <list>

#include "dvoronoi/common/point.hpp"

namespace dvoronoi {

    struct face_t;

    struct site_t {
        _internal::point2_t point{};
        face_t* face = nullptr;

        explicit site_t(_internal::scalar_t x, _internal::scalar_t y) : point(x, y) {}
    };

    struct vertex_t {
        _internal::point2_t point{};
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

    template<typename out_point_t>
    struct diag_traits {
        typedef out_point_t point_t;
        typedef _internal::scalar_t scalar_t;
        typedef site_t site_t;
        typedef face_t face_t;
        typedef vertex_t vertex_t;
        typedef half_edge_t half_edge_t;
    };

    template<typename diag_traits>
    struct diagram_t {
        typedef diag_traits::point_t point_t;
        typedef diag_traits::site_t site_t;
        typedef diag_traits::face_t face_t;
        typedef diag_traits::vertex_t vertex_t;
        typedef diag_traits::half_edge_t half_edge_t;

        std::vector<site_t> sites{};
        std::vector<face_t> faces{};
        std::list<vertex_t> vertices{};
        std::list<half_edge_t> half_edges{};

        vertex_t* create_vertex(const _internal::point2_t& point) {
            vertices.emplace_back(point);
            //vertices.back().it = std::prev(vertices.end());
            return &vertices.back();
        }

        half_edge_t* create_half_edge(face_t* face) {
            half_edges.emplace_back();
            half_edges.back().face = face;
            //half_edges.back().it = std::prev(half_edges.end());
            if (face->half_edge == nullptr)
                face->half_edge = &half_edges.back();
            return &half_edges.back();
        }
    };

} // namespace dvoronoi

#endif //DVORONOI_COMMON_DIAGRAM_HPP
