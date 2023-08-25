//
// Created by Daniel Secrieru on 08/05/2023.
//

#ifndef DVORONOI_BEACH_LINE_HPP
#define DVORONOI_BEACH_LINE_HPP

#include "dvoronoi/common/scalar.hpp"
#include "dvoronoi/common/util.hpp"
#include "arc_tree.hpp"
#include "arc.hpp"

namespace dvoronoi::fortune::_details {

    template<typename diag_traits>
    class beach_line_t : private arc_tree_t<data::arc_t<diag_traits>> {
    public:
        typedef data::arc_t<diag_traits> arc_t;
        typedef _internal::scalar_t scalar_t;
        typedef diag_traits::site_t site_t;

        beach_line_t() : arc_tree_t<arc_t>() {}
        ~beach_line_t() {
            arc_tree_t<arc_t>::free(this->_root);
            arc_tree_t<arc_t>::delete_arc(this->_nil);
            // std::cout << "[bl::dtor]: " << this->allocations << ", max: " << this->max_allocations << std::endl;;
        }

        beach_line_t(const beach_line_t&) = delete;
        beach_line_t(const beach_line_t&&) = delete;
        beach_line_t& operator=(const beach_line_t&) = delete;

        [[nodiscard]] bool empty() const { return is_nil(this->_root); }
        bool is_nil(const arc_t* arc) const { return arc_tree_t<arc_t>::is_nil(arc); }

        void set_root(site_t* site) {
            this->_root = create_arc(site, arc_t::side_t::Left);
            this->_root->color = arc_t::color_t::Black;
        }

        auto arc_above(const auto& point, auto sweep_y) const;
        auto break_arc(arc_t* arc, site_t* site);

        void remove(arc_t* arc) { arc_tree_t<arc_t>::remove(arc); }
        void delete_arc(arc_t* arc) { arc_tree_t<arc_t>::delete_arc(arc); }

        arc_t* leftmost_arc() const {
            auto x = this->_root;
            while (!is_nil(x->prev))
                x = x->prev;
            return x;
        }

    private:
        void replace(arc_t* arc, arc_t* other) { arc_tree_t<arc_t>::replace(arc, other); }
        void insert_before(arc_t* before, arc_t* arc) { arc_tree_t<arc_t>::insert_before(before, arc); }
        void insert_after(arc_t* after, arc_t* arc) { arc_tree_t<arc_t>::insert_after(after, arc); }

        arc_t* create_arc(site_t* site, typename arc_t::side_t side) {
            return arc_tree_t<arc_t>::new_arc(this->_nil, this->_nil, this->_nil, this->_nil, this->_nil, site, nullptr, nullptr, nullptr, arc_t::color_t::Red, side);
        }

        auto compute_breakpoint(const auto& p1, const auto& p2, auto sweep_y, typename arc_t::side_t side) const;
    };

    template<typename diag_traits>
    auto beach_line_t<diag_traits>::arc_above(const auto& point, auto sweep_y) const {
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

    template<typename diag_traits>
    auto beach_line_t<diag_traits>::break_arc(arc_t* arc, site_t* site) {
        auto middle_arc = create_arc(site, arc_t::side_t::Left);

        auto left_arc = create_arc(arc->site, arc_t::side_t::Left);
        left_arc->left_half_edge = arc->left_half_edge;

        auto right_arc = create_arc(arc->site, arc_t::side_t::Right);
        right_arc->right_half_edge = arc->right_half_edge;

        replace(arc, middle_arc);
        insert_before(middle_arc, left_arc);
        insert_after(middle_arc, right_arc);

        arc_tree_t<arc_t>::delete_arc(arc);

        return middle_arc;
    }

    template<typename diag_traits>
    auto beach_line_t<diag_traits>::compute_breakpoint(const auto& p1, const auto& p2, auto sweep_y, typename arc_t::side_t side) const {
        scalar_t x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;

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

} // namespace dvoronoi::fortune::_details

#endif //DVORONOI_BEACH_LINE_HPP
