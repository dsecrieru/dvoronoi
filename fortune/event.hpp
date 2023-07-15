//
// Created by Daniel Secrieru on 18/05/2023.
//

#ifndef DVORONOI_EVENT_HPP
#define DVORONOI_EVENT_HPP

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
        typedef data::arc_t<diag_traits> arc_t;

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

//        bool operator<(const event_t& o) const {
//            return y < o.y || (y == o.y && x < o.x);
//        }
    };

    template<typename T>
    bool operator<(const event_t<T>& lhs, const event_t<T>& rhs) {
        return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_EVENT_HPP
