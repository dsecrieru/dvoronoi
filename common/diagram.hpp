//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_COMMON_DIAGRAM_HPP
#define DVORONOI_COMMON_DIAGRAM_HPP

#include <vector>
#include <list>

namespace dvoronoi {

    template<typename point_t>
    struct face_t;

    template<typename point_t>
    struct site_t {
        point_t point{};
        face_t<point_t>* face = nullptr;
    };

    template<typename point_t>
    struct vertex_t {
        point_t point{};
    };

    template<typename point_t>
    struct half_edge_t {
        vertex_t<point_t>* orig = nullptr;
        vertex_t<point_t>* dest = nullptr;
        half_edge_t<point_t>* twin = nullptr;
        face_t<point_t>* face;

        half_edge_t<point_t>* prev = nullptr;
        half_edge_t<point_t>* next = nullptr;
    };

    template<typename point_t>
    struct face_t {
        site_t<point_t>* site = nullptr;
        half_edge_t<point_t>* half_edge = nullptr;
    };

    template<typename point_type>
    struct diagram_t {
        typedef point_type point_t;
        typedef decltype(point_t::x) scalar_t;
        typedef site_t<point_t> site_t;
        typedef face_t<point_t> face_t;
        typedef vertex_t<point_t> vertex_t;
        typedef half_edge_t<point_t> half_edge_t;

        std::vector<site_t> sites{};
        std::vector<face_t> faces{};
        std::list<vertex_t> vertices{};
        std::list<half_edge_t> half_edges{};

        vertex_t* create_vertex(const point_t& point) {
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
