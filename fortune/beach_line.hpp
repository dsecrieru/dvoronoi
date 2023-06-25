//
// Created by Daniel Secrieru on 08/05/2023.
//

#ifndef DVORONOI_BEACH_LINE_HPP
#define DVORONOI_BEACH_LINE_HPP

#include "arc_tree.hpp"
#include "arc.hpp"

namespace dvoronoi::fortune::_details {

    template<typename diag_t>
    class beach_line_t : private arc_tree_t<arc_t<diag_t>> {
    public:
        typedef arc_t<diag_t> arc_t;
        typedef diag_t::scalar_t scalar_t;
        typedef diag_t::site_t site_t;

        beach_line_t() : arc_tree_t<arc_t>() {}
        ~beach_line_t() {
            arc_tree_t<arc_t>::free(this->_root);
            delete this->_nil;
        }

        beach_line_t(const beach_line_t&) = delete;
        beach_line_t(const beach_line_t&&) = delete;
        beach_line_t& operator=(const beach_line_t&) = delete;

        bool is_nil(const arc_t* arc) const { return arc_tree_t<arc_t>::is_nil(arc); }

        void set_root(site_t* site) {
            this->_root = create_arc(site, arc_t::side_t::Left);
            this->_root->color = arc_t::color_t::Black;
        }

        auto arc_above(const auto& point, auto sweep_y) const;
        auto break_arc(arc_t* arc, site_t* site);

        void remove(arc_t* arc) { arc_tree_t<arc_t>::remove(arc); }

    private:
        void replace(arc_t* arc, arc_t* other) { arc_tree_t<arc_t>::replace(arc, other); }
        void insert_before(arc_t* before, arc_t* arc) { arc_tree_t<arc_t>::insert_before(before, arc); }
        void insert_after(arc_t* after, arc_t* arc) { arc_tree_t<arc_t>::insert_after(after, arc); }

        arc_t* create_arc(site_t* site, typename arc_t::side_t side) {
            return new arc_t{ this->_nil, this->_nil, this->_nil, this->_nil, this->_nil, site, nullptr, nullptr, {}, arc_t::color_t::Red, side };
        }

        auto compute_breakpoint(const auto& p1, const auto& p2, auto sweep_y, typename arc_t::side_t side) const;
    };

    template<typename diag_t>
    auto beach_line_t<diag_t>::arc_above(const auto& point, auto sweep_y) const {
        auto node = this->_root;
        bool found = false;

        while (!found) {
            auto breakpoint_left = -std::numeric_limits<scalar_t>::infinity();
            auto breakpoint_right = std::numeric_limits<scalar_t>::infinity();

            if (!is_nil(node->prev))
                breakpoint_left =  compute_breakpoint(node->prev->site->point, node->site->point, sweep_y, node->prev->side);
            if (!is_nil(node->next))
                breakpoint_right = compute_breakpoint(node->site->point, node->next->site->point, sweep_y, node->next->side);

            if (point.x < breakpoint_left)
                node = node->left;
            else if (point.x > breakpoint_right)
                node = node->right;
            else
                found = true;
        }

        return node;
    }

    template<typename diag_t>
    auto beach_line_t<diag_t>::break_arc(arc_t* arc, site_t* site) {
        auto middle_arc = create_arc(site, arc_t::side_t::Left);

        auto left_arc = create_arc(arc->site, arc_t::side_t::Left);
        left_arc->left_half_edge = arc->left_half_edge;

        auto right_arc = create_arc(arc->site, arc_t::side_t::Right);
        right_arc->right_half_edge = arc->right_half_edge;

        replace(arc, middle_arc);
        insert_before(middle_arc, left_arc);
        insert_after(middle_arc, right_arc);

        delete arc;

        return middle_arc;
    }

    template<typename diag_t>
    auto beach_line_t<diag_t>::compute_breakpoint(const auto& p1, const auto& p2, auto sweep_y, typename arc_t::side_t side) const {
        auto x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;

        if (util::eq(y1, y2)) {
            if (util::lt(x1, x2))
                return (x1 + x2) / 2;

            return side == arc_t::side_t::Left ?
                -std::numeric_limits<scalar_t>::infinity() :
                std::numeric_limits<scalar_t>::infinity();
        }

        if (util::eq(y1, sweep_y))
            return x1;

        if (util::eq(y2, sweep_y))
            return x2;

        auto d1 = 1.0f / (2.0f * (y1 - sweep_y));
        auto d2 = 1.0f / (2.0f * (y2 - sweep_y));
        auto a = d1 - d2;
        auto b = 2.0f * (x2 * d2 - x1 * d1);
        auto c = (y1 * y1 + x1 * x1 - sweep_y * sweep_y) * d1 - (y2 * y2 + x2 * x2 - sweep_y * sweep_y) * d2;
        auto delta = b * b - 4.0f * a * c;

        return (-b + std::sqrt(delta)) / (2.0f * a);
    }

//    template<typename point2d_t>
//    struct beachline_item {
//        point2d_t focus;
//        std::weak_ptr<bool> is_event_valid{};
//        std::shared_ptr<bool> marked_for_deletion{};
//
//        std::optional<std::size_t> left_half_edge{};
//        std::optional<std::size_t> right_half_edge{};
//    };
//
//    template<typename point2d_t>
//    void invalidate_circle_event(beachline_item<point2d_t>& arc) {
//        if (arc.is_event_valid.expired())
//            return;
//        *arc.is_event_valid.lock() = false;
//        arc.is_event_valid.reset();
//    }
//
//    template<typename point2d_t>
//    using beachline_implementation_t = std::variant<std::vector<beachline_item<point2d_t>>, std::set<beachline_item<point2d_t>>>;
//
//    template<typename point2d_t>
//    decltype(point2d_t::x) compute_break_point(const point2d_t& p1, const point2d_t& p2, auto sweep_y) {
//        auto x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
//
//        if (util::eq(y1, y2))
//            return (x1 + x2) / 2;
//
//        if (util::eq(y1, sweep_y))
//            return x1;
//
//        if (util::eq(y2, sweep_y))
//            return x2;
//
//        auto d1 = 1.0 / (2.0 * (y1 - sweep_y));
//        auto d2 = 1.0 / (2.0 * (y2 - sweep_y));
//        auto a = d1 - d2;
//        auto b = 2.0 * (x2 * d2 - x1 * d1);
//        auto c = (y1 * y1 + x1 * x1 - sweep_y * sweep_y) * d1 - (y2 * y2 + x2 * x2 - sweep_y * sweep_y) * d2;
//        auto delta = b * b - 4.0 * a * c;
//
//        return (-b - std::sqrt(delta)) / (2.0 * a);
//    }
//
//    template<typename point2d_t>
//    std::optional<point2d_t> compute_convergence_point(const point2d_t& p1, const point2d_t& p2, const point2d_t& p3) {
//        auto v1 = util::ortho(util::diff(p1, p2));
//        auto v2 = util::ortho(util::diff(p2, p3));
//        auto delta = util::mult(0.5, util::diff(p3, p1));
//        auto denom = util::determinant(v1, v2);
//
//        if (util::is_zero(denom))
//            return std::nullopt;
//
//        auto t = util::determinant(delta, v2) / denom;
//        auto center = util::add(util::mult(0.5, util::add(p1, p2)),
//                                util::mult(t, v1));
//
//        return center;
//    }
//
//    template<typename point2d_t>
//    std::optional<event_t<point2d_t>> maybe_create_event(const point2d_t& p1, const point2d_t& p2, const point2d_t& p3, auto sweep_y) {
//        auto convergence_point = compute_convergence_point(p1, p2, p3);
//
//        if (!convergence_point.has_value())
//            return std::nullopt;
//
//        auto r = util::distance(convergence_point.value(), p1);
//        auto event_y = convergence_point.value().y - r;
//
//        if (!util::lt(event_y, sweep_y))
//            return std::nullopt;
//
//        bool left_bp_moving_right = util::lt(p1.y, p2.y);
//        bool right_bp_moving_right = util::lt(p2.y, p3.y);
//        auto left_initial_x = left_bp_moving_right ? p1.x : p2.x;
//        auto right_initial_x = right_bp_moving_right ? p2.x : p3.x;
//
//        bool diverging = !left_bp_moving_right && right_bp_moving_right;
//        if (diverging)
//            return std::nullopt;
//
//        bool left_bp_moving_towards_cp =
//                (left_bp_moving_right && util::lt(left_initial_x, convergence_point.value().x)) ||
//                (!left_bp_moving_right && util::gt(left_initial_x, convergence_point.value().x));
//        bool right_bp_moving_towards_cp =
//                (right_bp_moving_right && util::lt(right_initial_x, convergence_point.value().x)) ||
//                (!right_bp_moving_right && util::gt(right_initial_x, convergence_point.value().x));
//
//        if (!left_bp_moving_towards_cp || !right_bp_moving_towards_cp)
//            return std::nullopt;
//
//        return std::make_optional<event_t<point2d_t>>(convergence_point.value().x, event_y, convergence_point.value(), p1, p2, p3);
//    }
//
//    template<typename point2d_t>
//    void maybe_add_circle_event(auto left_iter, auto mid_iter, auto right_iter, auto sweep_y, std::size_t idx, event_queue_t<point2d_t>& event_queue) {
//        auto new_event = maybe_create_event(left_iter->focus, mid_iter->focus, right_iter->focus, sweep_y);
//        if (!new_event.has_value())
//            return;
//
//        auto& data = new_event.value().circle_data.value();
//
//        mid_iter->is_event_valid = data.is_valid;
//        mid_iter->marked_for_deletion = std::make_shared<bool>(false);
//        data.delete_arc = mid_iter->marked_for_deletion;
//        data.last_known_index = idx;
//
//        event_queue.push(std::move(new_event.value()));
//    }
//
//    template<typename point2d_t>
//    auto locate_arc_above_bs(std::vector<beachline_item<point2d_t>>& beachline, const point2d_t& new_site) {
//        auto base = beachline.begin();
//        std::size_t len = beachline.size();
//
//        while (len > 1) {
//            std::size_t half = len / 2;
//            auto break_point_x = compute_break_point((base + half - 1)->focus, (base + half)->focus, new_site.y);
//            base += (util::lt(break_point_x, new_site.x)) * half;
//            len -= half;
//        }
//
//        return base;
//    }
//
//    template<typename point2d_t>
//    struct insert_visitor {
//        typedef beachline_item<point2d_t> beachline_item_t;
//
//        const point2d_t& new_site;
//        event_queue_t<point2d_t>& event_queue;
//        diagram_t<point2d_t>& diagram;
//
//        void operator()(std::vector<beachline_item_t>& storage) {
//            auto arc_above = locate_arc_above_bs(storage, new_site);
//
//            invalidate_circle_event(*arc_above);
//
//            auto left_arc = storage.insert(arc_above, { beachline_item_t(*arc_above), beachline_item_t(new_site) });
//            auto mid_arc = std::next(left_arc);
//            auto right_arc = std::next(mid_arc);
//
//            diagram.half_edges.push_back({});
//            diagram.half_edges.push_back({});
//
//            left_arc->left_half_edge = right_arc->left_half_edge;
//            left_arc->right_half_edge = diagram.half_edges.size() - 2;
//            mid_arc->left_half_edge = mid_arc->right_half_edge = diagram.half_edges.size() - 1;
//            right_arc->left_half_edge = left_arc->right_half_edge;
//
//            if (left_arc > storage.begin())
//                maybe_add_circle_event<point2d_t>(std::prev(left_arc), left_arc, mid_arc, new_site.y,
//                                                  left_arc - storage.begin(), event_queue);
//
//            if (right_arc < std::prev(storage.end()))
//                maybe_add_circle_event<point2d_t>(mid_arc, right_arc, std::next(right_arc), new_site.y,
//                                                  right_arc - storage.begin(), event_queue);
//        }
//
//        void operator()(std::set<beachline_item_t>& storage) {
//        }
//    };
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    template<typename point2d_t>
//    bool is_valid(const typename event_t<point2d_t>::vec_circle_data_t& data) {
//        assert (data.is_valid);
//        return *(data.is_valid);
//    }
//
//    template<typename point2d_t>
//    std::optional<std::size_t> locate_deletion_index(const std::vector<beachline_item<point2d_t>>& storage, std::size_t idx_hint) {
//        constexpr std::size_t search_chunk = 32;
//
//        std::size_t size = storage.size();
//
//        bool left = false;
//        std::size_t left_count = 1;
//        std::size_t right_count = 0;
//
//        while (true) {
//            int start = left ?
//                    (std::max<int>(idx_hint - left_count * search_chunk, 0)) :
//                    (std::min(idx_hint + right_count * search_chunk, size));
//            std::size_t end = std::min(start + search_chunk, size);
//
//            for (int i = start; i < end; ++i) {
//                if (!storage[i].marked_for_deletion)
//                    continue;
//                if (*storage[i].marked_for_deletion)
//                    return i;
//            }
//
//            left_count += left;
//            right_count += !left;
//            left = !left;
//        }
//
//        assert (false);
//        return std::nullopt;
//    }
//
//    template<typename point2d_t>
//    struct remove_visitor {
//        typedef event_t<point2d_t> event_t;
//        typedef beachline_item<point2d_t> beachline_item_t;
//
//        point2d_t event_point;
//        const typename event_t::vec_circle_data_t& data;
//        event_queue_t<point2d_t>& event_queue;
//        diagram_t<point2d_t>& diagram;
//
//        void operator()(std::vector<beachline_item_t>& storage) {
//            if (!*data.is_valid)
//                return;
//
//            diagram.vertices.emplace_back(event_point);
//            std::size_t vertex_idx = diagram.vertices.size() - 1;
//
//            assert (!data.delete_arc.expired());
//            *data.delete_arc.lock() = true;
//
//            auto arc_idx = locate_deletion_index(storage, data.last_known_index.value());
//            assert (arc_idx.has_value());
//            auto del_iter = storage.begin() + arc_idx.value();
//
//            diagram.half_edges[del_iter->left_half_edge.value()].dest = vertex_idx;
//            diagram.half_edges[del_iter->right_half_edge.value()].orig = vertex_idx;
//
//            invalidate_circle_event(*std::prev(del_iter));
//            invalidate_circle_event(*std::next(del_iter));
//
//            auto right_arc = storage.erase(del_iter);
//            auto left_arc = std::prev(right_arc);
//
//            diagram.half_edges[left_arc->right_half_edge.value()].orig = vertex_idx;
//            diagram.half_edges[right_arc->left_half_edge.value()].dest = vertex_idx;
//
//            diagram.half_edges.push_back({});
//            diagram.half_edges.push_back({});
//
//            left_arc->right_half_edge = diagram.half_edges.size() - 2;
//            right_arc->left_half_edge = diagram.half_edges.size() - 1;
//
//            diagram.half_edges[left_arc->right_half_edge.value()].dest = vertex_idx;
//            diagram.half_edges[right_arc->left_half_edge.value()].orig = vertex_idx;
//
//            if (left_arc > storage.begin())
//                maybe_add_circle_event<point2d_t>(std::prev(left_arc), left_arc, right_arc, event_point.y,
//                                                  left_arc - storage.begin(), event_queue);
//
//            if (right_arc < std::prev(storage.end()))
//                maybe_add_circle_event<point2d_t>(left_arc, right_arc, std::next(right_arc), event_point.y,
//                                                  right_arc - storage.begin(), event_queue);
//        }
//
//        void operator()(std::set<beachline_item_t>& storage) {
//        }
//    };
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    template<typename point2d_t>
//    struct first_insert_visitor {
//        const point2d_t& new_site;
//
//        void operator()(std::vector<beachline_item<point2d_t>>& storage) {
//            storage.emplace_back(new_site);
//        }
//
//        void operator()(std::set<beachline_item<point2d_t>>& storage) {
//        }
//    };

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_BEACH_LINE_HPP
