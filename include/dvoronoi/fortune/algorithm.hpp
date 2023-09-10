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
#include "details.hpp"

namespace dvoronoi::fortune {

template<typename face_user_data = none, typename half_edge_user_data = none>
class algorithm {
public:
    typedef face_user_data face_user_data_t;
    typedef half_edge_user_data half_edge_user_data_t;
    typedef voronoi_diagram_t<face_user_data_t, half_edge_user_data_t> diagram_t;

    auto generate(const auto& sites, const config_t& config = config_t{}) {
        assert(!sites.empty());

        auto diagram = std::make_unique<diagram_t>(sites.size());

        priority_queue_t<_details::event_t<diag_traits_t>> event_queue(sites.size());

        for (std::size_t i = 0; i < sites.size(); ++i) {
            diagram->sites.emplace_back(i, sites[i].x, sites[i].y);
            diagram->faces.emplace_back(&diagram->sites.back());
            diagram->sites.back().face = &diagram->faces.back();

            event_queue.emplace(&diagram->sites.back());
        }

        _details::beach_line_t<diag_traits_t> beach_line;

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

        if (config.bounding_box.has_value())
            bound<face_user_data_t, half_edge_user_data_t>(*diagram, config.bounding_box.value(), beach_line);

        return diagram;
    }

private:
    typedef diag_traits<face_user_data_t, half_edge_user_data_t> diag_traits_t;
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
