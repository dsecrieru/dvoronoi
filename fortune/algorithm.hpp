//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_FORTUNE_ALGORITHM_HPP
#define DVORONOI_FORTUNE_ALGORITHM_HPP

#include <vector>
#include <generator>

#include "dvoronoi/common/diagram.hpp"
#include "details.hpp"

namespace dvoronoi::fortune {

template<typename point2d_t>
auto generate(const std::vector<point2d_t>& sites) -> diagram_t<point2d_t> {
    _details::event_queue_t<point2d_t> event_queue(_details::event_cmp<point2d_t>);

    for (const auto& site : sites)
        event_queue.emplace(site);

    return _details::generate(event_queue);
}

template<typename point2d_t>
auto generate(std::generator<point2d_t>& site_generator) -> diagram_t<point2d_t> {
    _details::event_queue_t<point2d_t> event_queue(_details::event_cmp<point2d_t>);

    for (auto site : site_generator)
        event_queue.emplace(site);

    return _details::generate(event_queue);
}

} // namespace dvoronoi::fortune

#endif //DVORONOI_FORTUNE_ALGORITHM_HPP
