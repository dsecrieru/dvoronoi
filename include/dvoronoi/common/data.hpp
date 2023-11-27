//
// Created by Daniel Secrieru on 01/08/2023.
//

#ifndef EMERGENT_DATA_HPP
#define EMERGENT_DATA_HPP

#include <optional>
#include <numeric>

#include "point.hpp"
#include "util.hpp"

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
        std::size_t index;
        point_t point{};
    };

    template<typename FACE>
    struct gen_half_edge_t {
        std::size_t index;
        vertex_t* orig = nullptr;
        vertex_t* dest = nullptr;
        gen_half_edge_t* twin = nullptr;
        FACE* face = nullptr;

        gen_half_edge_t* prev = nullptr;
        gen_half_edge_t* next = nullptr;
    };
    using half_edge_t = gen_half_edge_t<face_t>;

    struct face_t {
        site_t* site = nullptr;
        half_edge_t* half_edge = nullptr;
    };

    struct triangle_t {
        vertex_t* voronoi_vertex = nullptr;
        gen_half_edge_t<triangle_t>* half_edge = nullptr;
    };

    static auto get_face_vertices(const face_t& face) {
        std::vector<point_t> points;

        auto he = face.half_edge;
        do {
            points.emplace_back(he->orig->point);
            he = he->next;
        } while (he != face.half_edge);

        return points;
    }

    static auto calculate_face_centroid(const face_t& face) {
        auto vertices = get_face_vertices(face);

        auto sum = std::accumulate(vertices.begin(), vertices.end(), point_t{scalar_t{}, scalar_t {}}, [](const point_t& a, const point_t& b) {
            return a + b;
        });

        return sum / static_cast<double>(vertices.size());
    }

    static bool contains(const auto& face, const point_t& p, bool closed_boundary = true) {
        auto count = 0;
        auto he = face.half_edge;
        do {
            if ((he->orig->point.y <= p.y && he->dest->point.y > p.y) ||
                (he->orig->point.y > p.y && he->dest->point.y <= p.y)) {

                double vt = (p.y - he->orig->point.y) / (he->dest->point.y - he->orig->point.y);

                if (p.x > he->orig->point.x + vt * (he->dest->point.x - he->orig->point.x)) {
                    ++count;
                } else if (p.x == he->orig->point.x + vt * (he->dest->point.x - he->orig->point.x)) {
                    return closed_boundary;
                }
            }

            he = he->next;
        } while (he != face.half_edge);

        return count % 2 == 1;
    }

    static std::optional<point_t> find_intersection(const auto& face, const point_t& p) {
        const auto& center = face.site->point;
        auto dir_center = p - center;

        auto he = face.half_edge;
        do {
            auto current_he = he;
            he = he->next;

            auto dir_he = current_he->dest->point - current_he->orig->point;
            auto cross = dir_center.det(dir_he);

            auto he_orig_to_center = current_he->orig->point - center;

            if (cross == 0) {
                if (he_orig_to_center.det(dir_center) == 0 && current_he->orig->point.is_between(center, p)) {
                    return current_he->orig->point;
                }
                continue;
            }

            auto t = he_orig_to_center.det(dir_he) / cross;
            auto u = he_orig_to_center.det(dir_center) / cross;

            if (util::between_eq(t, 0.0, 1.0) && util::between_eq(u, 0.0, 1.0)) {
                return point_t{ center.x + t * dir_center.x, center.y + t * dir_center.y };
            }
        } while (he != face.half_edge);

        return std::nullopt;
    }

} // namespace dvoronoi::data

#endif //EMERGENT_DATA_HPP
