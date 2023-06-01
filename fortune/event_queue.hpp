//
// Created by Daniel Secrieru on 18/05/2023.
//

#ifndef DVORONOI_EVENT_QUEUE_HPP
#define DVORONOI_EVENT_QUEUE_HPP

#include <variant>
#include <deque>
#include <queue>

namespace dvoronoi::fortune::_details {

    enum class event_type {
        site,
        circle
    };

    template<typename point2d_t>
    struct event_t {
        event_type type;

        point2d_t event_point;

        struct vec_circle_data_t {
            point2d_t convergence;
            point2d_t p1, p2, p3;
            std::shared_ptr<bool> is_valid;
            std::weak_ptr<bool> delete_arc;
            std::optional<std::size_t> last_known_index;

            explicit vec_circle_data_t(const point2d_t& cp, const point2d_t& p1, const point2d_t& p2, const point2d_t& p3)
                : convergence(cp), p1(p1), p2(p2), p3(p3), is_valid(new bool(true)) {}
        };
        std::optional<vec_circle_data_t> circle_data;

        explicit event_t(const point2d_t& p)
            : type(event_type::site), event_point(p) {}
        explicit event_t(decltype(point2d_t::x) ev_x, decltype(point2d_t::y) ev_y, const point2d_t& cp, const point2d_t& p1, const point2d_t& p2, const point2d_t& p3)
            : type(event_type::circle), event_point{ev_x, ev_y}, circle_data(std::make_optional<vec_circle_data_t>(cp, p1, p2, p3)) {}
    };

    template<typename point2d_t>
    auto event_cmp = [](const event_t<point2d_t> &e1, const event_t<point2d_t> &e2) {
        return e1.event_point.y > e2.event_point.y + std::numeric_limits<decltype(point2d_t::y)>::epsilon() ||
               (e1.event_point.y == e2.event_point.y && e1.event_point.x < e2.event_point.x - std::numeric_limits<decltype(point2d_t::y)>::epsilon());
    };

    template<typename point2d_t>
    using event_queue_storage_t = std::deque<event_t<point2d_t>>;

    template<typename point2d_t>
    using event_queue_t = std::priority_queue<event_t<point2d_t>, event_queue_storage_t<point2d_t>, decltype(event_cmp<point2d_t>)>;

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_EVENT_QUEUE_HPP
