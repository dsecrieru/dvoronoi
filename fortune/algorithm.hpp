//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_FORTUNE_ALGORITHM_HPP
#define DVORONOI_FORTUNE_ALGORITHM_HPP

#include <vector>
//#include <generator>

#include "dvoronoi/common/diagram.hpp"
#include "details.hpp"

namespace dvoronoi::fortune {

template<typename point_t>
auto generate(const std::vector<point_t>& sites, const point_t& lt, const point_t& rb) {
    typedef diag_traits<point_t> traits;
    typedef diagram_t<traits> diag_t;
    diag_t diagram;
    diagram.sites.reserve(sites.size());
    diagram.faces.reserve(sites.size());

//    auto event_queue = _details::create_event_queue<traits>(sites.size());
    std::priority_queue<_details::event_t<traits>> event_queue;

    for (const auto& site : sites) {
        diagram.sites.emplace_back(site.x, site.y);
        diagram.faces.emplace_back(&diagram.sites.back());
        diagram.sites.back().face = &diagram.faces.back();

        event_queue.emplace(&diagram.sites.back());
    }

    _details::generate<traits>(lt, rb, diagram, event_queue);

    return diagram;
}

//template<typename point_t>
//auto generate(std::generator<point_t>& site_generator, const point_t& lt, const point_t& rb) {
//    typedef diagram_t<point_t> diag_t;
//    diag_t diagram;
//
//    _details::event_queue_t<diag_t> event_queue(_details::event_compare<_details::event_t<diag_t>>);
//
//    for (auto site : site_generator) {
//        diagram.sites.emplace_back(site);
//        event_queue.emplace(&diagram.sites.back());
//    }
//
//    _details::generate(lt, rb, diagram, event_queue);
//
//    return diagram;
//}

} // namespace dvoronoi::fortune

#endif //DVORONOI_FORTUNE_ALGORITHM_HPP
