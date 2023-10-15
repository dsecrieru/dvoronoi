//
// Created by Daniel Secrieru on 01/08/2023.
//

#ifndef EMERGENT_DATA_HPP
#define EMERGENT_DATA_HPP

#include <optional>

#include "point.hpp"
#include "util.hpp"

namespace dvoronoi::data {
    typedef _internal::scalar_t scalar_t;
    typedef _internal::point2_t point_t;

    template<typename face_user_data, typename half_edge_user_data>
    struct face_t;

    template<typename face_user_data, typename half_edge_user_data>
    struct site_t {
        std::size_t index;
        point_t point{};
        face_t<face_user_data, half_edge_user_data>* face = nullptr;

        explicit site_t(std::size_t i, scalar_t x, scalar_t y) : index(i), point(x, y) {}
    };

    struct vertex_t {
        std::size_t index;
        point_t point{};
    };

    template<typename face_user_data, typename half_edge_user_data>
    struct half_edge_t {
        std::size_t index;
        vertex_t* orig = nullptr;
        vertex_t* dest = nullptr;
        half_edge_t* twin = nullptr;
        face_t<face_user_data, half_edge_user_data>* face = nullptr;

        half_edge_t* prev = nullptr;
        half_edge_t* next = nullptr;

        half_edge_user_data data{};
    };

    template<typename face_user_data>
    struct half_edge_t<face_user_data, none> {
        std::size_t index;
        vertex_t* orig = nullptr;
        vertex_t* dest = nullptr;
        half_edge_t* twin = nullptr;
        face_t<face_user_data, none>* face = nullptr;

        half_edge_t* prev = nullptr;
        half_edge_t* next = nullptr;
    };

    template<typename face_user_data, typename half_edge_user_data>
    struct face_t {
        site_t<face_user_data, half_edge_user_data>* site = nullptr;
        half_edge_t<face_user_data, half_edge_user_data>* half_edge = nullptr;

        face_user_data data{};
    };

    template<typename half_edge_user_data>
    struct face_t<none, half_edge_user_data> {
        site_t<none, half_edge_user_data>* site = nullptr;
        half_edge_t<none, half_edge_user_data>* half_edge = nullptr;
    };

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
