//
// Created by Daniel Secrieru on 07/07/2023.
//

#ifndef DVORONOI_BOUND_HPP
#define DVORONOI_BOUND_HPP

#include <array>
#include <unordered_map>

#include "dvoronoi/common/diagram.hpp"

namespace dvoronoi::fortune::_details {

    template<typename point_t>
    struct linked_vertex_t {
        dvoronoi::data::half_edge_t<point_t>* prev_half_edge = nullptr;
        dvoronoi::data::vertex_t<point_t>* vertex = nullptr;
        dvoronoi::data::half_edge_t<point_t>* next_half_edge = nullptr;
    };

    template<typename point_t>
    using linked_vertices_t = std::list<linked_vertex_t<point_t>>;
    template<typename point_t>
    using vertices_t = std::unordered_map<std::size_t, std::array<linked_vertex_t<point_t>*, 8>>;

    template<typename point_t>
    bool bound(auto& diag, box_t box, auto& beach_line) {
        bool all_bounded = true;

        for (const auto& vertex : diag.vertices) {
            box.left = std::min(static_cast<_internal::scalar_t>(vertex.point.x), box.left);
            box.bottom = std::min(static_cast<_internal::scalar_t>(vertex.point.y), box.bottom);
            box.right = std::max(static_cast<_internal::scalar_t>(vertex.point.x), box.right);
            box.top = std::max(static_cast<_internal::scalar_t>(vertex.point.y), box.top);
        }

        linked_vertices_t<point_t> linked_vertices;
        vertices_t<point_t> vertices;

        if (!beach_line.empty()) {
            auto arc = beach_line.leftmost_arc();
            while (!beach_line.is_nil(arc->next)) {
                all_bounded = bound_edge(box, arc, arc->next, linked_vertices, vertices, diag) && all_bounded;
                arc = arc->next;
            }
        }

        for (auto& kv : vertices)
            all_bounded = add_corners(box, linked_vertices, kv.second, diag) && all_bounded;

        for (const auto& kv : vertices)
            join_half_edges(kv.first, kv.second, diag);

        return all_bounded;
    }

    struct intersection_t
    {
        std::size_t side;
        _internal::point2_t point;
    };

    intersection_t first_intersection(const auto& box, const auto& origin, const auto& direction) {
        auto intersection = intersection_t{};

        auto t = std::numeric_limits<_internal::scalar_t>::infinity();

        if (direction.x > 0.0) {
            t = (box.right - origin.x) / direction.x;
            intersection.side = box_side::Right;
            intersection.point = origin + t * direction;
        } else if (direction.x < 0.0) {
            t = (box.left - origin.x) / direction.x;
            intersection.side = box_side::Left;
            intersection.point = origin + t * direction;
        }

        if (direction.y > 0.0) {
            auto u = (box.top - origin.y) / direction.y;
            if (u < t) {
                intersection.side = box_side::Top;
                intersection.point = origin + u * direction;
            }
        } else if (direction.y < 0.0) {
            auto u = (box.bottom - origin.y) / direction.y;
            if (u < t) {
                intersection.side = box_side::Bottom;
                intersection.point = origin + u * direction;
            }
        }

        return intersection;
    }

    bool bound_edge(const auto& box, auto* left_arc, auto* right_arc, auto& linked_vertices, auto& vertices, auto& diag) {
        bool success = true;

        _internal::point2_t left_point{left_arc->site->point.x, left_arc->site->point.y};
        _internal::point2_t right_point{right_arc->site->point.x, right_arc->site->point.y};

        auto direction = (left_point - right_point).ortho();
        auto origin = (left_point + right_point) * 0.5;
        auto intersection = first_intersection(box, origin, direction);

        auto vertex = diag.create_vertex(intersection.point);
        set_dest(left_arc, right_arc, vertex);

        if (vertices.find(left_arc->site->index) == vertices.end())
            vertices[left_arc->site->index].fill(nullptr);
        if (vertices.find(right_arc->site->index) == vertices.end())
            vertices[right_arc->site->index].fill(nullptr);

        success = vertices[left_arc->site->index][2 * intersection.side + 1] == nullptr && success;
        success = vertices[right_arc->site->index][2 * intersection.side] == nullptr && success;

        linked_vertices.emplace_back(nullptr, vertex, left_arc->right_half_edge);
        vertices[left_arc->site->index][2 * intersection.side + 1] = &linked_vertices.back();
        linked_vertices.emplace_back(right_arc->left_half_edge, vertex, nullptr);
        vertices[right_arc->site->index][2 * intersection.side] = &linked_vertices.back();

        return success;
    }

    bool add_corners(const auto& box, auto& linked_vertices, auto& cell_vertices, auto& diag) {
        auto success = true;

        for (std::size_t i = 0; i < 5; ++i) {
            std::size_t side = i % 4;

            if (cell_vertices[2 * side] == nullptr && cell_vertices[2 * side + 1] != nullptr) {
                auto corner = diag.create_corner(box, side);
                linked_vertices.emplace_back(nullptr, corner, nullptr);

                auto prev_side = (side + 3) % 4;
                success = cell_vertices[2 * prev_side + 1] == nullptr && success;

                cell_vertices[2 * prev_side + 1] = &linked_vertices.back();
                cell_vertices[2 * side] = &linked_vertices.back();
            } else if (cell_vertices[2 * side] != nullptr && cell_vertices[2 * side + 1] == nullptr) {
                std::size_t next_side = (side + 1) % 4;

                auto corner = diag.create_corner(box, next_side);
                linked_vertices.emplace_back(nullptr, corner, nullptr);

                success = cell_vertices[2 * next_side] == nullptr && success;

                cell_vertices[2 * side + 1] = &linked_vertices.back();
                cell_vertices[2 * next_side] = &linked_vertices.back();
            }
        }

        return success;
    }

    void join_half_edges(std::size_t i, const auto& cell_vertices, auto& diag) {
        for (std::size_t side = 0; side < 4; ++side) {
            if (cell_vertices[2 * side] == nullptr)
                continue;

            auto half_edge = diag.create_half_edge(&diag.faces[i]);
            half_edge->orig = cell_vertices[2 * side]->vertex;
            half_edge->dest = cell_vertices[2 * side + 1]->vertex;
            cell_vertices[2 * side]->next_half_edge = half_edge;
            half_edge->prev = cell_vertices[2 * side]->prev_half_edge;
            if (cell_vertices[2 * side]->prev_half_edge != nullptr)
                cell_vertices[2 * side]->prev_half_edge->next = half_edge;
            cell_vertices[2 * side + 1]->prev_half_edge = half_edge;
            half_edge->next = cell_vertices[2 * side + 1]->next_half_edge;
            if (cell_vertices[2 * side + 1]->next_half_edge != nullptr)
                cell_vertices[2 * side + 1]->next_half_edge->prev = half_edge;
        }
    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_BOUND_HPP
