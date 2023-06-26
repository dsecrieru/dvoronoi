//
// Created by Daniel Secrieru on 05/05/2023.
//

#ifndef DVORONOI_DETAILS_HPP
#define DVORONOI_DETAILS_HPP

#include <variant>
#include <deque>
#include <queue>
#include <cassert>

#include "event_queue.hpp"
#include "beach_line.hpp"

namespace dvoronoi::fortune::_details {

    template<typename arc_t>
    void add_edge(arc_t* left, arc_t* right, auto& diagram) {
        left->right_half_edge = diagram.create_half_edge(left->site->face);
        right->left_half_edge = diagram.create_half_edge(right->site->face);

        left->right_half_edge->twin = right->left_half_edge;
        right->left_half_edge->twin = left->right_half_edge;
    }

    template<typename point_t>
    auto compute_convergence_point(const point_t& p1, const point_t& p2, const point_t& p3) -> std::optional<point_t> {
        auto v1 = (p1 - p2).ortho();
        auto v2 = (p2 - p3).ortho();
        auto delta = (p3 - p1) * 0.5;
        auto denom = v1.det(v2);

        if (util::is_zero(denom))
            return std::nullopt;

        auto t = delta.det(v2) / denom;
        auto center = (p1 + p2) * 0.5 + v1 * t;

        return center;
    }

    template<typename event_t>
    void maybe_add_circle_event(auto* left, auto* middle, auto* right, auto sweep_y, auto& event_queue) {
        const auto& p1 = left->site->point;
        const auto& p2 = middle->site->point;
        const auto& p3 = right->site->point;

        auto convergence_point = compute_convergence_point(p1, p2, p3);

        if (!convergence_point.has_value())
            return;

        auto r = convergence_point.value().dist(p1);
        auto event_y = convergence_point.value().y - r;

        if (!util::lt(event_y, sweep_y))
            return;

        bool left_bp_moving_right = util::lt(p1.y, p2.y);
        bool right_bp_moving_right = util::lt(p2.y, p3.y);
        bool diverging = !left_bp_moving_right && right_bp_moving_right;
        if (diverging)
            return;

        auto left_initial_x = left_bp_moving_right ? p1.x : p2.x;
        auto right_initial_x = right_bp_moving_right ? p2.x : p3.x;

        bool left_bp_moving_towards_cp =
                (left_bp_moving_right && util::lt(left_initial_x, convergence_point.value().x)) ||
                (!left_bp_moving_right && util::gt(left_initial_x, convergence_point.value().x));
        bool right_bp_moving_towards_cp =
                (right_bp_moving_right && util::lt(right_initial_x, convergence_point.value().x)) ||
                (!right_bp_moving_right && util::gt(right_initial_x, convergence_point.value().x));

        if (!left_bp_moving_towards_cp || !right_bp_moving_towards_cp)
            return;

        auto new_event = std::make_unique<event_t>(event_y, convergence_point.value(), middle);
        middle->is_event_valid = new_event->is_valid;
        event_queue.push(std::move(new_event));
    }

    void invalidate_circle_event(auto* arc) {
        if (arc->is_event_valid.expired())
            return;

        *arc->is_event_valid.lock() = false;
        arc->is_event_valid.reset();
    }

    template<typename event_t>
    void handle_site_event(const event_t& event, auto& beach_line, auto& diagram, auto& event_queue) {
        auto arc_above = beach_line.arc_above(event.site->point, event.y);
        invalidate_circle_event(arc_above);

        auto middle_arc = beach_line.break_arc(arc_above, event.site);
        auto left_arc = middle_arc->prev;
        auto right_arc = middle_arc->next;

        add_edge(left_arc, middle_arc, diagram);
        middle_arc->right_half_edge = middle_arc->left_half_edge;
        right_arc->left_half_edge = left_arc->right_half_edge;

        if (!beach_line.is_nil(left_arc->prev))
            maybe_add_circle_event<event_t>(left_arc->prev, left_arc, middle_arc, event.y, event_queue);
        if (!beach_line.is_nil(right_arc->next))
            maybe_add_circle_event<event_t>(middle_arc, right_arc, right_arc->next, event.y, event_queue);
    }

    void set_dest(auto* left, auto* right, auto* vertex) {
        left->right_half_edge->orig = vertex;
        right->left_half_edge->dest = vertex;
    }

    void set_orig(auto* left, auto* right, auto* vertex) {
        left->right_half_edge->dest = vertex;
        right->left_half_edge->orig = vertex;
    }

    void set_prev_half_edge(auto* prev, auto* next) {
        prev->next = next;
        next->prev = prev;
    }

    void remove_arc_and_update_diag(auto* arc, auto* vertex, auto& beach_line, auto& diagram) {
        set_dest(arc->prev, arc, vertex);
        set_dest(arc, arc->next, vertex);

        arc->left_half_edge->next = arc->right_half_edge;
        arc->right_half_edge->prev = arc->left_half_edge;

        beach_line.remove(arc);

        auto prev_half_edge = arc->prev->right_half_edge;
        auto next_half_edge = arc->next->left_half_edge;

        add_edge(arc->prev, arc->next, diagram);
        set_orig(arc->prev, arc->next, vertex);
        set_prev_half_edge(arc->prev->right_half_edge, prev_half_edge);
        set_prev_half_edge(next_half_edge, arc->next->left_half_edge);

        delete arc;
    }

    template<typename event_t>
    void handle_circle_event(const event_t& event, auto& beach_line, auto& diagram, auto& event_queue) {
        if (!*event.is_valid)
            return;

        auto vertex = diagram.create_vertex(event.convergence);

        auto arc = event.arc;
        auto left_arc = arc->prev;
        auto right_arc = arc->next;

        invalidate_circle_event(left_arc);
        invalidate_circle_event(right_arc);

        remove_arc_and_update_diag(arc, vertex, beach_line, diagram);

        if (!beach_line.is_nil(left_arc->prev))
            maybe_add_circle_event<event_t>(left_arc->prev, left_arc, right_arc, event.y, event_queue);
        if (!beach_line.is_nil(right_arc->next))
            maybe_add_circle_event<event_t>(left_arc, right_arc, right_arc->next, event.y, event_queue);
    }

    template<typename diag_traits>
    void generate(const auto& lt, const auto& rb, auto& diagram, auto& event_queue) {
        assert (!event_queue.empty());

//        typedef event_t<point2d_t> event_t;
//
//        diagram_t<point2d_t> diagram;
//        diagram.vertices.reserve(2 * event_queue.size() - 2);
//        diagram.half_edges.reserve(3 * event_queue.size() - 4);
//
//        beachline_implementation_t<point2d_t> beachline = std::vector<beachline_item<point2d_t>>();
//        std::get<std::vector<beachline_item<point2d_t>>>(beachline).reserve(2 * event_queue.size() - 1);
        beach_line_t<diag_traits> beach_line;
//
        {
            auto first_site_event = event_queue.pop();
            beach_line.set_root(first_site_event->site);
        }

//        std::size_t max_events = 0;
        while (!event_queue.empty()) {
//            if (event_queue.size() > max_events)
//                max_events = event_queue.size();

            auto event = event_queue.pop();

            if (event->type == event_type::site) {
                handle_site_event(*event, beach_line, diagram, event_queue);
            } else {
                handle_circle_event(*event, beach_line, diagram, event_queue);
            }
        }

//        std::cout << "inserted events = " << max_events << std::endl;

        bool bound_success = bound(lt, rb, diagram, beach_line);
    }

    bool bound(const auto& lt, const auto& rb, auto& diag, auto& beach_line) {
        bool success = true;
        return success;
    }

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_DETAILS_HPP
