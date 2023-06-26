//
// Created by Daniel Secrieru on 18/05/2023.
//

#ifndef DVORONOI_EVENT_QUEUE_HPP
#define DVORONOI_EVENT_QUEUE_HPP

#include "dvoronoi/common/point.hpp"
#include "arc.hpp"

namespace dvoronoi::fortune::_details {

    enum class event_type {
        site,
        circle
    };

    template<typename diag_traits>
    struct event_t {
        typedef _internal::point2_t point_t;
        typedef diag_traits::scalar_t scalar_t;
        typedef diag_traits::site_t* site_h;
        typedef arc_t<diag_traits> arc_t;

        event_type type;

        scalar_t x{};
        scalar_t y{};
        std::size_t index{};

        site_h site = nullptr;

        point_t convergence{};
        arc_t* arc = nullptr;

        explicit event_t(site_h site)
            : type(event_type::site), x(site->point.x), y(site->point.y), site(site) {}
        explicit event_t(scalar_t y, const point_t& cp, arc_t* arc)
            : type(event_type::circle), x(cp.x), y(y), convergence(cp), arc(arc) {}

        bool operator<(const event_t& o) const {
            return y < o.y || (y == o.y && x < o.x);
        }
    };

//    template<typename T>
//    auto event_compare = [](const event_t<T>& e1, const event_t<T>& e2) {
//        return e1.y < e2.y || (e1.y == e2.y && e1.x < e2.x);
//    };
//
//    template<typename T>
//    using event_queue_t = std::priority_queue<event_t<T>, std::vector<event_t<T>>, decltype(event_compare<T>)>;
//
//    template<typename T>
//    auto create_event_queue(std::size_t size_hint) {
//        std::vector<event_t<T>> storage;
//        storage.reserve(size_hint + size_hint / 100);
//        return event_queue_t<T>(event_compare<T>, std::move(storage));
//    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_EVENT_QUEUE_HPP
