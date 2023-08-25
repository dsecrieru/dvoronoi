//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_COMMON_DIAGRAM_HPP
#define DVORONOI_COMMON_DIAGRAM_HPP

#include <vector>
#include <list>
#include <cassert>

#include "scalar.hpp"
#include "data.hpp"
#include "point.hpp"
#include "box.hpp"
#include "util.hpp"

//#define DIAG_USE_PMR
#ifdef DIAG_USE_PMR
#include "tracing_resource.hpp"
#endif

namespace dvoronoi {
    template<typename point_t>
    struct diag_traits {
        typedef point_t out_point_t;
        typedef data::site_t<point_t> site_t;
        typedef data::face_t<point_t> face_t;
        typedef data::vertex_t<point_t> vertex_t;
        typedef data::half_edge_t<point_t> half_edge_t;
    };
}

namespace dvoronoi::voronoi {

    template<typename point_t>
    class diagram_t {
    public:
        typedef diag_traits<point_t>::site_t site_t;
        typedef diag_traits<point_t>::face_t face_t;
        typedef diag_traits<point_t>::vertex_t vertex_t;
        typedef diag_traits<point_t>::half_edge_t half_edge_t;
        typedef std::vector<std::vector<std::size_t>> triangulation_t;
        typedef std::vector<std::size_t> convex_hull_t;

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
        std::unique_ptr<convex_hull_t> convex_hull{};

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

        vertex_t* create_vertex(const _internal::point2_t& p) {
            vertices.emplace_back(p.x, p.y);
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

        void compute_convex_hull() {
            auto n = sites.size();
            assert(n > 3);

            convex_hull = std::make_unique<convex_hull_t>(2 * n);

            auto sorted(sites);
            std::ranges::sort(sorted, [](const site_t& s1, const site_t& s2) {
                return s1.point.x < s2.point.x || (s1.point.x == s2.point.x && s1.point.y < s2.point.y);
            });

            std::size_t k = 0;

            for (size_t i = 0; i < n; ++i) {
                while (k >= 2 && util::cross(sites[(*convex_hull)[k - 2]].point, sites[(*convex_hull)[k - 1]].point, sorted[i].point) <= 0) --k;
                (*convex_hull)[k++] = sorted[i].index;
            }

            for (size_t i = n - 1, t = k + 1; i > 0; --i) {
                while (k >= t && util::cross(sites[(*convex_hull)[k - 2]].point, sites[(*convex_hull)[k - 1]].point, sorted[i - 1].point) <= 0) --k;
                (*convex_hull)[k++] = sorted[i - 1].index;
            }

            convex_hull->resize(k - 1);
        }

    }; // class diagram_t

} // namespace dvoronoi::voronoi

namespace dvoronoi {
    template<typename point_t>
    using voronoi_diagram_t = voronoi::diagram_t<point_t>;

    auto compute_lloyd_relaxation(const auto& diag) -> std::vector<typename decltype(diag)::out_point_t> {
        std::vector<typename decltype(diag)::out_point_t> sites{};

        for (const auto& face : diag.faces) {
            _internal::scalar_t area = 0.0;
            _internal::point2_t centroid{};

            auto he = face.half_edge;
            do {
                auto det = he->orig->point.det(he->dest->point);
                area += det;
                centroid += (he->orig->point + he->dest->point) * det;

                he = he->next;
            } while (he != face.half_edge);

            area *= 0.5;
            centroid *= 1.0 / (6.0 * area);

            sites.emplace_back(centroid.x, centroid.y);
        }

        return sites;
    }
}

#endif //DVORONOI_COMMON_DIAGRAM_HPP
