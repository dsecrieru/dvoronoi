//
// Created by Daniel Secrieru on 28/09/2023.
//

#ifndef DVORONOI_CLIPPING_HPP
#define DVORONOI_CLIPPING_HPP

#include <unordered_set>
#include <unordered_map>
#include <array>

#include "dvoronoi/common/diagram.hpp"

namespace dvoronoi::voronoi {

    bool clip(auto& diag, const box_t& box) {
        using half_edge_t = data::half_edge_t;

        bool success = true;

        auto removed_vertices = std::unordered_set<std::size_t>{};
        auto removed_half_edges = std::unordered_set<std::size_t>{};
        auto new_vertices = std::list<data::vertex_t>{};
        auto new_half_edges = std::list<half_edge_t>{};
        auto processed_half_edges = std::unordered_set<half_edge_t*>();

        auto create_vertex = [&diag, &new_vertices](const auto& point) {
            return diag.create_vertex(point, new_vertices, diag.vertices.size() + new_vertices.size());
        };
        auto create_half_edge = [&diag, &new_half_edges](auto* face) {
            return diag.create_half_edge(face, new_half_edges, diag.half_edges.size() + new_half_edges.size());
        };

        auto link = [&diag, &box, &new_vertices, &create_half_edge](half_edge_t* start, std::size_t start_side, half_edge_t* end, std::size_t end_side) {
            auto half_edge = start;
            auto side = start_side;
            while (side != end_side) {
                side = (side + 1) % 4;
                half_edge->next = create_half_edge(start->face);
                half_edge->next->prev = half_edge;
                half_edge->next->orig = half_edge->dest;
                half_edge->next->dest = diag.create_corner(box, side, new_vertices, diag.vertices.size() + new_vertices.size());
                half_edge = half_edge->next;
            }
            half_edge->next = create_half_edge(start->face);
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

            half_edge_t* incoming_half_edge = nullptr;
            half_edge_t* outgoing_half_edge = nullptr;
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
                        half_edge->next = nullptr;
                        removed_half_edges.emplace(half_edge->index);
                    } else if (intersections_count == 2) {
                        removed_vertices.emplace(half_edge->orig->index);

                        if (processed_half_edges.find(half_edge->twin) != processed_half_edges.end()) {
                            half_edge->orig = half_edge->twin->dest;
                            half_edge->dest = half_edge->twin->orig;
                        } else {
                            half_edge->orig = create_vertex(intersections[0].point);
                            half_edge->dest = create_vertex(intersections[1].point);
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
                            half_edge->dest = create_vertex(intersections[0].point);

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
                            half_edge->orig = create_vertex(intersections[0].point);

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

        auto swapped_vertices = std::unordered_map<std::size_t, std::size_t>{};

        auto maybe_swap_vertex = [&diag, &removed_vertices, &swapped_vertices](data::vertex_t*& vertex) {
            if (vertex->index < diag.vertices.size())
                return;

            const auto& swapped_iter = swapped_vertices.find(vertex->index);
            if (swapped_iter != swapped_vertices.end()) {
                vertex = &diag.vertices[swapped_iter->second];
                return;
            }

            if (removed_vertices.empty())
                assert (diag.vertices.size() < diag.vertices.capacity());

            std::size_t available;
            if (!removed_vertices.empty()) {
                available = *removed_vertices.begin();
                removed_vertices.erase(removed_vertices.begin());

                diag.vertices[available].point = vertex->point;
            }
            else
                available = diag.create_vertex(vertex->point)->index;

            swapped_vertices[vertex->index] = available;

            vertex = &diag.vertices[available];
        };

        auto swapped_half_edges = std::unordered_map<std::size_t, std::size_t>{};

        auto maybe_swap_half_edge = [&diag, &removed_half_edges, &swapped_half_edges](half_edge_t*& half_edge, const auto& is_invalid) {
            if (is_invalid(half_edge))
                return;

            const auto& swapped_iter = swapped_half_edges.find(half_edge->index);
            if (swapped_iter != swapped_half_edges.end()) {
                half_edge = &diag.half_edges[swapped_iter->second];
                return;
            }

            if (removed_half_edges.empty())
                return;

            auto available = *removed_half_edges.begin();
            removed_half_edges.erase(removed_half_edges.begin());

            swapped_half_edges[half_edge->index] = available;

            auto& available_he = diag.half_edges[available];

            available_he = *half_edge;
            available_he.index = available;

            if (half_edge->face->half_edge->index == half_edge->index)
                half_edge->face->half_edge = &available_he;

            half_edge->next = nullptr;
            half_edge = &available_he;
        };

        for (const auto& he : processed_half_edges) {
            maybe_swap_vertex(he->orig);
            maybe_swap_vertex(he->dest);
        }
        for (auto& he : new_half_edges) {
            maybe_swap_vertex(he.orig);
            maybe_swap_vertex(he.dest);
        }

        auto index_from_existing_invalid = [max_index = diag.half_edges.size()](const auto* half_edge) {
            return !half_edge || half_edge->index < max_index;
        };

        for (auto& he : new_half_edges) {
            maybe_swap_half_edge(he.next, index_from_existing_invalid);
            maybe_swap_half_edge(he.prev, index_from_existing_invalid);
        }
        for (auto& he : diag.half_edges) {
            maybe_swap_half_edge(he.next, index_from_existing_invalid);
            maybe_swap_half_edge(he.prev, index_from_existing_invalid);
        }

        if (removed_half_edges.empty())
            return success;

        swapped_half_edges.clear();

        auto all_indices_valid = [](const auto* half_edge) {
            return !half_edge;
        };

        while (true) {
            auto to_be_swapped = std::optional<std::size_t>{};

            auto he_iter = diag.half_edges.rbegin();
            while (he_iter != diag.half_edges.rend()) {
                if (!he_iter->next) {
                    removed_half_edges.erase(he_iter->index);
                    diag.half_edges.pop_back();
                    he_iter = diag.half_edges.rbegin();
                    continue;
                }

                to_be_swapped = he_iter->index;
                break;
            }

            if (removed_half_edges.empty())
                break;
            assert (to_be_swapped.has_value());

            for (auto& he : diag.half_edges) {
                if (!he.next)
                    continue;

                if (he.next->index == to_be_swapped.value())
                    maybe_swap_half_edge(he.next, all_indices_valid);

                if (he.prev->index == to_be_swapped.value())
                    maybe_swap_half_edge(he.prev, all_indices_valid);
            }
        }

        return success;
    }

}

#endif //DVORONOI_CLIPPING_HPP
