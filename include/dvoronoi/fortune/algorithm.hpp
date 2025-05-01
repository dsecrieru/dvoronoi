//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_FORTUNE_ALGORITHM_HPP
#define DVORONOI_FORTUNE_ALGORITHM_HPP

#include <vector>
//#include <generator>
#include <cassert>

#include "dvoronoi/common/diagram.hpp"
#include "dvoronoi/common/priority_queue.hpp"
#include "dvoronoi/common/clipping.hpp"
#include "dvoronoi/common/pair_hash.hpp"

#include "details.hpp"

namespace dvoronoi::fortune {

class algorithm {
public:
    typedef voronoi_diagram_t diagram_t;
    using voronoi_diagram_h = std::unique_ptr<diagram_t>;
    typedef delaunay_diagram_t delaunay_diagram_t;
    using delaunay_diagram_h = std::unique_ptr<delaunay_diagram_t>;

    static auto generate(const auto& sites, const config_t& config = config_t{}) {
        assert(!sites.empty());

        auto diagram = std::make_unique<diagram_t>(sites.size());

        priority_queue_t<_details::event_t<diag_traits>> event_queue(sites.size());

        for (std::size_t i = 0; i < sites.size(); ++i) {
            diagram->sites.emplace_back(i, sites[i].x, sites[i].y);
            diagram->faces.emplace_back(&diagram->sites.back());
            diagram->sites.back().face = &diagram->faces.back();

            event_queue.emplace(&diagram->sites.back());
        }

        _details::beach_line_t<diag_traits> beach_line;

        {
            auto first_site_event = event_queue.pop();
            beach_line.set_root(first_site_event->site);
        }

        while (!event_queue.empty()) {
            auto event = event_queue.pop();

            if (event->type == _details::event_type::site)
                handle_site_event(*event, beach_line, *diagram, event_queue);
            else
                handle_circle_event(*event, beach_line, *diagram, event_queue);
        }

        if (config.bounding_box.has_value()) {
            bound(*diagram, config.bounding_box.value(), beach_line);
            if (config.clip)
                clip(*diagram, config.bounding_box.value());
        }

        return diagram;
    }

    static bool clip(auto& diag, const box_t& box) { return voronoi::clip(diag, box); }

    static auto generate_delaunay(const voronoi_diagram_h& voronoi_diagram) {
        auto diagram = std::make_unique<delaunay_diagram_t>(voronoi_diagram->sites.size());

        std::unordered_set<std::size_t> created_triangles;
        std::unordered_map<std::size_t, std::size_t> created_vertices;
        std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t, pair_hash> created_half_edges;

        auto vertex_helper = [&diagram, &created_vertices](const data::site_t& site) {
            auto [v_iter, v_inserted] = created_vertices.emplace(site.index, diagram->vertices.size());
            if (!v_inserted)
                return &diagram->vertices[v_iter->second];

            return diagram->create_vertex(site.point);
        };

        auto half_edge_helper = [&diagram, &created_half_edges](const auto& s_orig, auto* v_orig, const auto& s_dest, auto* v_dest, auto* triangle) {
            auto new_he = diagram->create_half_edge(v_orig, v_dest, triangle);
            created_half_edges.emplace(std::make_pair(s_orig->index, s_dest->index), new_he->index);

            auto he21_iter = created_half_edges.find(std::make_pair(s_dest->index, s_orig->index));
            if (he21_iter != created_half_edges.end()) {
                new_he->twin = &diagram->half_edges[he21_iter->second];
                diagram->half_edges[he21_iter->second].twin = new_he;
            }

            return new_he;
        };

        for (const auto& face : voronoi_diagram->faces) {
            const auto& s1 = face.site;

            auto v1 = vertex_helper(*s1);

            auto edge = face.half_edge;
            do {
                const auto current_edge = edge;
                const auto next_edge = current_edge->next;
                edge = next_edge;

                if (!current_edge->twin || !next_edge->twin)
                    continue;

                const auto vertex = current_edge->dest;
                auto [triangle_iter, triangle_inserted] = created_triangles.emplace(vertex->index);
                if (!triangle_inserted)
                    continue;

                auto new_triangle = diagram->create_triangle(vertex);

                const auto& s2 = current_edge->twin->face->site;
                const auto& s3 = next_edge->twin->face->site;

                auto v2 = vertex_helper(*s2);
                auto v3 = vertex_helper(*s3);

                auto he12 = half_edge_helper(s1, v1, s2, v2, new_triangle);
                auto he23 = half_edge_helper(s2, v2, s3, v3, new_triangle);
                auto he31 = half_edge_helper(s3, v3, s1, v1, new_triangle);

                he12->next = he23;
                he23->next = he31;
                he31->next = he12;

                he12->prev = he31;
                he31->prev = he23;
                he23->prev = he12;
            } while (edge != face.half_edge);
        }

        return diagram;
    }
};

//template<typename point_t>
//auto generate(std::generator<point_t>& site_generator, const config_t& config = config_t{}) {
//    typedef diag_traits<point_t> traits;
//    typedef diagram_t<traits> diag_t;
//    diag_t diagram;
//
//    priority_queue_t<_details::event_t<traits>> event_queue;
//
//    std::size_t i = 0;
//    for (auto site : site_generator) {
//        diagram.sites.emplace_back(++i, sites[i].x, sites[i].y);
//        event_queue.emplace(&diagram.sites.back());
//    }
//
//    _details::generate(config, diagram, event_queue);
//
//    return diagram;
//}

} // namespace dvoronoi::fortune

#endif //DVORONOI_FORTUNE_ALGORITHM_HPP
