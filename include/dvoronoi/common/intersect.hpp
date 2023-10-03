//
// Created by Daniel Secrieru on 28/09/2023.
//

#ifndef DVORONOI_INTERSECT_HPP
#define DVORONOI_INTERSECT_HPP

#include <unordered_set>
#include <queue>
#include <array>

#include "dvoronoi/common/diagram.hpp"

namespace dvoronoi::voronoi {

    bool intersect(auto& diag, const box_t& box) {
        const auto start = std::chrono::steady_clock::now();
        bool success = true;

        auto removed_vertices = std::queue<std::size_t>{};
        auto removed_half_edges = std::queue<std::size_t>{};
        std::list<data::vertex_t>* new_vertices = new std::list<data::vertex_t>;
        std::list<data::half_edge_t>* new_half_edges = new std::list<data::half_edge_t>;
        auto processed_half_edges = std::unordered_set<data::half_edge_t*>();

        auto link = [&diag, &box, new_vertices, new_half_edges](data::half_edge_t* start, std::size_t start_side, data::half_edge_t* end, std::size_t end_side) {
            auto half_edge = start;
            auto side = start_side;
            while (side != end_side) {
                side = (side + 1) % 4;
                half_edge->next = diag.create_half_edge(start->face, *new_half_edges);
                half_edge->next->prev = half_edge;
                half_edge->next->orig = half_edge->dest;
                half_edge->next->dest = diag.create_corner(box, side, *new_vertices);
                half_edge = half_edge->next;
            }
            half_edge->next = diag.create_half_edge(start->face, *new_half_edges);
            half_edge->next->prev = half_edge;
            end->prev = half_edge->next;
            half_edge->next->next = end;
            half_edge->next->orig = half_edge->dest;
            half_edge->next->dest = end->orig;
        };

        for (const auto& site : diag.sites) {
            auto half_edge = site.face->half_edge;

            auto inside = box.contains(half_edge->orig->point);
            auto outer_component_dirty = !inside;

            data::half_edge_t* incoming_half_edge = nullptr;
            data::half_edge_t* outgoing_half_edge = nullptr;
            auto incoming_side = box_side::Left;
            auto outgoing_side = box_side::Left;

            do {
                auto intersections = std::array<dvoronoi::intersection_t, 2>{};
                auto intersections_count = box.intersections(half_edge->orig->point, half_edge->dest->point, intersections);
                auto next_inside = box.contains(half_edge->dest->point);
                auto next_half_edge = half_edge->next;

                if (!inside && !next_inside) {
                    if (intersections_count == 0) {
                        removed_vertices.emplace(half_edge->orig->index);
                        half_edge->in_use = false;
                        removed_half_edges.emplace(half_edge->index);
                    } else if (intersections_count == 2) {
                        removed_vertices.emplace(half_edge->orig->index);

                        if (processed_half_edges.find(half_edge->twin) != processed_half_edges.end()) {
                            half_edge->orig = half_edge->twin->dest;
                            half_edge->dest = half_edge->twin->orig;
                        } else {
                            half_edge->orig = diag.create_vertex(intersections[0].point, *new_vertices);
                            half_edge->dest = diag.create_vertex(intersections[1].point, *new_vertices);
                        }

                        if (outgoing_half_edge != nullptr)
                            link(outgoing_half_edge, outgoing_side, half_edge, intersections[0].side);

                        if (incoming_half_edge == nullptr) {
                            incoming_half_edge = half_edge;
                            incoming_side = intersections[0].side;
                        }

                        outgoing_half_edge = half_edge;
                        outgoing_side = intersections[1].side;

                        processed_half_edges.emplace(half_edge);
                    } else
                        success = false; // this is impossible?
                } else if (inside && !next_inside) {
                    if (intersections_count >= 1) {
                        if (processed_half_edges.find(half_edge->twin) != processed_half_edges.end())
                            half_edge->dest = half_edge->twin->orig;
                        else
                            half_edge->dest = diag.create_vertex(intersections[0].point, *new_vertices);

                        outgoing_half_edge = half_edge;
                        outgoing_side = intersections[0].side;

                        processed_half_edges.emplace(half_edge);
                    } else
                        success = false;
                } else if (!inside && next_inside) {
                    if (intersections_count >= 1) {
                        removed_vertices.emplace(half_edge->orig->index);

                        if (processed_half_edges.find(half_edge->twin) != processed_half_edges.end())
                            half_edge->orig = half_edge->twin->dest;
                        else
                            half_edge->orig = diag.create_vertex(intersections[0].point, *new_vertices);

                        if (outgoing_half_edge != nullptr)
                            link(outgoing_half_edge, outgoing_side, half_edge, intersections[0].side);

                        if (incoming_half_edge == nullptr) {
                            incoming_half_edge = half_edge;
                            incoming_side = intersections[0].side;
                        }

                        processed_half_edges.emplace(half_edge);
                    } else
                        success = false;
                }

                half_edge = next_half_edge;
                inside = next_inside;
            } while (half_edge != site.face->half_edge);

            if (outer_component_dirty && incoming_half_edge != nullptr)
                link(outgoing_half_edge, outgoing_side, incoming_half_edge, incoming_side);

            if (outer_component_dirty)
                site.face->half_edge = incoming_half_edge;
        }

        const auto end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

        return success;
    }

}

#endif //DVORONOI_INTERSECT_HPP
