//
// Created by Daniel Secrieru on 05/05/2023.
//

#ifndef DVORONOI_DETAILS_HPP
#define DVORONOI_DETAILS_HPP

#include <variant>
#include <deque>
#include <queue>

#include "event_queue.hpp"
#include "beachline.hpp"

namespace dvoronoi::fortune::_details {

    template<typename point2d_t>
    void insert_arc(beachline_implementation_t<point2d_t>& beachline, const point2d_t& new_site,
                    event_queue_t<point2d_t>& event_queue, diagram_t<point2d_t>& diagram) {
        insert_visitor<point2d_t> inserter{ new_site, event_queue, diagram };
        std::visit(inserter, beachline);
    }

    template<typename point2d_t>
    void remove_arc(beachline_implementation_t<point2d_t>& beachline, const event_t<point2d_t>& event,
                    event_queue_t<point2d_t>& event_queue, diagram_t<point2d_t>& diagram) {
        remove_visitor<point2d_t> remover{ event.event_point, event.circle_data.value(), event_queue, diagram };
        std::visit(remover, beachline);
    }

    template<typename point2d_t>
    auto generate(event_queue_t<point2d_t>& event_queue) -> diagram_t<point2d_t> {
        assert (!event_queue.empty());

        typedef event_t<point2d_t> event_t;

        diagram_t<point2d_t> diagram;
        diagram.vertices.reserve(2 * event_queue.size() - 2);
        diagram.half_edges.reserve(3 * event_queue.size() - 4);

        beachline_implementation_t<point2d_t> beachline = std::vector<beachline_item<point2d_t>>();
        std::get<std::vector<beachline_item<point2d_t>>>(beachline).reserve(2 * event_queue.size() - 1);

        {
            auto first_site_event = event_queue.top();
            event_queue.pop();
            first_insert_visitor<point2d_t> inserter{ first_site_event.event_point };
            std::visit(inserter, beachline);
        }

        while (!event_queue.empty()) {
            auto event = event_queue.top();
            event_queue.pop();

            if (event.type == event_type::site) {
                insert_arc(beachline, event.event_point,event_queue, diagram);
            } else {
                remove_arc(beachline, event, event_queue, diagram);
            }
        }

        return diagram;
    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_DETAILS_HPP
