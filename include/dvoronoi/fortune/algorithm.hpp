//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_FORTUNE_ALGORITHM_HPP
#define DVORONOI_FORTUNE_ALGORITHM_HPP

#include <vector>
//#include <generator>

#include "dvoronoi/common/diagram.hpp"
#include "dvoronoi/common/priority_queue.hpp"
#include "details.hpp"

namespace dvoronoi::fortune {

template<typename point_t>
auto generate(const std::vector<point_t>& sites, const config_t& config = config_t{}) {
    typedef diag_traits<point_t> traits;
    typedef diagram_t<traits> diag_t;
    auto diagram = std::make_unique<diag_t>(sites.size());
    
    priority_queue_t<_details::event_t<traits>> event_queue(sites.size());

    for (std::size_t i = 0; i < sites.size(); ++i) {
        diagram->sites.emplace_back(i, sites[i].x, sites[i].y);
        diagram->faces.emplace_back(&diagram->sites.back());
        diagram->sites.back().face = &diagram->faces.back();

        event_queue.emplace(&diagram->sites.back());
    }

    _details::generate<traits>(config, *diagram, event_queue);

    return diagram;
}

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