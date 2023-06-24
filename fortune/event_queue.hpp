//
// Created by Daniel Secrieru on 18/05/2023.
//

#ifndef DVORONOI_EVENT_QUEUE_HPP
#define DVORONOI_EVENT_QUEUE_HPP

#include <variant>
#include <deque>
#include <queue>

#include "dvoronoi/common/diagram.hpp"
#include "dvoronoi/common/util.hpp"
#include "dvoronoi/common/point.hpp"
#include "arc.hpp"

namespace dvoronoi::fortune::_details {

    enum class event_type {
        site,
        circle
    };

    template<typename diag_t>
    struct event_t {
        typedef diag_t::point_t point_t;
        typedef diag_t::scalar_t scalar_t;
        typedef diag_t::site_t site_t;
        typedef site_t* site_h;
        typedef arc_t<diag_t> arc_t;

        event_type type;

        scalar_t x;
        scalar_t y;

        typedef site_h site_data_t;

        struct circle_data_t {
            std::shared_ptr<bool> is_valid;
            point_t convergence;
            arc_t* arc;

            explicit circle_data_t(const point_t& cp, arc_t* arc)
                : is_valid(new bool(true)), convergence(cp), arc(arc) {}
        };
        std::variant<site_data_t, circle_data_t> data;

        explicit event_t(site_h site)
            : type(event_type::site), x(site->point.x), y(site->point.y), data(site) {}
        explicit event_t(scalar_t y, const point_t& cp, arc_t* arc)
            : type(event_type::circle), x(cp.x), y(y), data{ std::in_place_type<circle_data_t>, cp, arc } {}
    };

    template<typename ev_t>
    auto event_compare = [](const ev_t& e1, const ev_t& e2) {
        constexpr auto eps = std::numeric_limits<decltype(ev_t::y)>::epsilon();
        return e1.y < e2.y - eps || (std::fabs(e1.y - e2.y) < eps && e1.x < e2.x - eps);
    };

    template<typename T>
    using event_queue_t = std::priority_queue<event_t<T>, std::vector<event_t<T>>, decltype(event_compare<event_t<T>>)>;

    template<typename T>
    auto create_event_queue(std::size_t size_hint) {
        std::vector<event_t<T>> storage;
        storage.reserve(size_hint);
        return event_queue_t<T>(event_compare<event_t<T>>, std::move(storage));
    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_EVENT_QUEUE_HPP
