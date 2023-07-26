//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_COMMON_DIAGRAM_HPP
#define DVORONOI_COMMON_DIAGRAM_HPP

#include <vector>
#include <list>

#include "point.hpp"
#include "box.hpp"

//#define DIAG_USE_PMR
#ifdef DIAG_USE_PMR
#include "tracing_resource.hpp"
#endif

namespace dvoronoi {
    namespace data {
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
    } // namespace data

    template<typename out_point_t>
    struct diag_traits {
        typedef out_point_t out_point_t;
        typedef data::scalar_t scalar_t;
        typedef data::site_t site_t;
        typedef data::face_t face_t;
        typedef data::vertex_t vertex_t;
        typedef data::half_edge_t half_edge_t;
    };

    template<typename diag_traits>
    class diagram_t {
    public:
        typedef diag_traits::site_t site_t;
        typedef diag_traits::face_t face_t;
        typedef diag_traits::vertex_t vertex_t;
        typedef diag_traits::half_edge_t half_edge_t;
        typedef std::vector<std::vector<std::size_t>> triangulation_t;

#ifdef DIAG_USE_PMR
    private:
        //memory_management::tracing_resource _tracing_vert{"vert", std::pmr::null_memory_resource()};
        std::unique_ptr<std::byte[]> _vert_buf;
        std::pmr::monotonic_buffer_resource _vert_res;
        //memory_management::tracing_resource _tracing_he{"he  ", std::pmr::null_memory_resource()};
        std::unique_ptr<std::byte[]> _he_buf;
        std::pmr::monotonic_buffer_resource _he_res;
#endif

    public:
        std::vector<site_t> sites{};
        std::vector<face_t> faces{};
#ifdef DIAG_USE_PMR
        std::pmr::vector<vertex_t> vertices{&_vert_res}; // requires pointer stability, so no re-allocation allowed
        std::pmr::vector<half_edge_t> half_edges{&_he_res}; // requires pointer stability, so no re-allocation allowed
#else
        std::vector<vertex_t> vertices{}; // requires pointer stability, so no re-allocation allowed
        std::vector<half_edge_t> half_edges{}; // requires pointer stability, so no re-allocation allowed
#endif
        std::unique_ptr<triangulation_t> triangulation{};

        explicit diagram_t(std::size_t n)
#ifdef DIAG_USE_PMR
            : _vert_buf(new std::byte[(2 * n + 10) * sizeof(vertex_t)]), _vert_res(&_vert_buf[0], (2 * n + 10) * sizeof(vertex_t), std::pmr::null_memory_resource()/*&_tracing_vert*/)
            , _he_buf(new std::byte[6 * n * sizeof(half_edge_t)]), _he_res(&_he_buf[0], 6 * n * sizeof(half_edge_t), std::pmr::null_memory_resource()/*&_tracing_he*/)
#endif
        {
            sites.reserve(n);
            faces.reserve(n);
            vertices.reserve(2 * n + 10);
            half_edges.reserve(6 * n);
        }

        vertex_t* create_vertex(const data::point_t& point) {
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

        vertex_t* create_corner(const box_t& box, std::size_t side) {
            switch (side) {
                case box_side::Left:
                    return create_vertex({ box.left, box.top });
                case box_side::Bottom:
                    return create_vertex({ box.left, box.bottom });
                case box_side::Right:
                    return create_vertex({ box.right, box.bottom });
                case box_side::Top:
                    return create_vertex({ box.right, box.top });
                default:
                    return nullptr;
            }
        }

        void generate_delaunay() {
            triangulation = std::make_unique<triangulation_t>(sites.size());

            for(auto i = 0; i < sites.size(); ++i) {
                const auto& face = faces[i];
                auto half_edge = face.half_edge;
                while (half_edge->prev != nullptr) {
                    half_edge = half_edge->prev;
                    if (half_edge == face.half_edge)
                        break;
                }
                while (half_edge != nullptr) {
                    if (half_edge->twin != nullptr)
                        (*triangulation)[i].push_back(half_edge->twin->face->site->index);
                    half_edge = half_edge->next;
                    if (half_edge == face.half_edge)
                        break;
                }
            }
        }
    }; // class diagram_t

} // namespace dvoronoi

#endif //DVORONOI_COMMON_DIAGRAM_HPP
