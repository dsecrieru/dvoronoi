//
// Created by Daniel Secrieru on 08/06/2023.
//

#ifndef DVORONOI_ARC_TREE_HPP
#define DVORONOI_ARC_TREE_HPP

//#define BL_USE_PMR
#ifdef BL_USE_PMR
#include "tracing_resource.hpp"
#endif

namespace dvoronoi::fortune::_details {

    template<typename arc_t>
    class arc_tree_t {
    protected:
#ifdef BL_USE_PMR
        using allocator_type = std::pmr::polymorphic_allocator<>;
        //memory_management::tracing_resource _tracing_res{"beach_line"};
        std::pmr::monotonic_buffer_resource _res{/*&_tracing_res*/};
        std::pmr::unsynchronized_pool_resource _pool{&_res};
        allocator_type _allocator{&_pool};
        // std::size_t allocations;
        // std::size_t max_allocations;
#endif

        arc_t* _nil;
        arc_t* _root;

        void free(arc_t* arc) {
            if (is_nil(arc))
                return;

            free(arc->left);
            free(arc->right);

            delete_arc(arc);
        }

        template<typename... Args>
        arc_t* new_arc(Args&&... args) {
            // ++this->allocations;
            // this->max_allocations = std::max(this->allocations, this->max_allocations);
#ifdef BL_USE_PMR
            return _allocator.new_object<arc_t>(std::forward<Args>(args)...);
#else
            return new arc_t { std::forward<Args>(args)... };
#endif
        }
        
        void delete_arc(arc_t* arc) {
#ifdef BL_USE_PMR
            _allocator.delete_object(arc);
#else
            delete arc;
#endif
            // --allocations;
        }

        arc_tree_t() : /*allocations(1), max_allocations(1), */_nil(new_arc()), _root(_nil) {}

        bool is_nil(const arc_t* arc) const { return arc == _nil; }

        void replace(arc_t* arc, arc_t* other) {
            transplant(arc, other);

            other->left = arc->left;
            other->right = arc->right;

            if (!is_nil(other->left))
                other->left->parent = other;
            if (!is_nil(other->right))
                other->right->parent = other;

            other->prev = arc->prev;
            other->next = arc->next;

            if (!is_nil(other->prev))
                other->prev->next = other;
            if (!is_nil(other->next))
                other->next->prev = other;

            other->color = arc->color;
        }

        void remove(arc_t* arc)
        {
            auto y = arc;
            auto y_original_color = y->color;
            arc_t* x;

            if (is_nil(arc->left)) {
                x = arc->right;
                transplant(arc, arc->right);
            } else if (is_nil(arc->right)) {
                x = arc->left;
                transplant(arc, arc->left);
            } else {
                y = minimum(arc->right);
                y_original_color = y->color;
                x = y->right;
                if (y->parent == arc)
                    x->parent = y;
                else {
                    transplant(y, y->right);
                    y->right = arc->right;
                    y->right->parent = y;
                }
                transplant(arc, y);
                y->left = arc->left;
                y->left->parent = y;
                y->color = arc->color;
            }

            if (y_original_color == arc_t::color_t::Black)
                remove_fixup(x);

            if (!is_nil(arc->prev))
                arc->prev->next = arc->next;
            if (!is_nil(arc->next))
                arc->next->prev = arc->prev;
        }

        void insert_before(arc_t* before, arc_t* arc) {
            if (is_nil(before->left)) {
                before->left = arc;
                arc->parent = before;
            } else {
                before->prev->right = arc;
                arc->parent = before->prev;
            }

            arc->prev = before->prev;
            if (!is_nil(arc->prev))
                arc->prev->next = arc;
            arc->next = before;
            before->prev = arc;

            insert_fixup(arc);
        }

        void insert_after(arc_t* after, arc_t* arc) {
            if (is_nil(after->right)) {
                after->right = arc;
                arc->parent = after;
            } else {
                after->next->left = arc;
                arc->parent = after->next;
            }

            arc->next = after->next;
            if (!is_nil(arc->next))
                arc->next->prev = arc;
            arc->prev = after;
            after->next = arc;

            insert_fixup(arc);
        }

        void transplant(arc_t* u, arc_t* v) {
            if (is_nil(u->parent))
                _root = v;
            else if (u == u->parent->left)
                u->parent->left = v;
            else
                u->parent->right = v;
            v->parent = u->parent;
        }

        void insert_fixup(arc_t* z) {
            while (z->parent->color == arc_t::color_t::Red) {
                if (z->parent == z->parent->parent->left) {
                    auto y = z->parent->parent->right;

                    if (y->color == arc_t::color_t::Red) {
                        z->parent->color = arc_t::color_t::Black;
                        y->color = arc_t::color_t::Black;
                        z->parent->parent->color = arc_t::color_t::Red;
                        z = z->parent->parent;
                    } else {
                        if (z == z->parent->right) {
                            z = z->parent;
                            left_rotate(z);
                        }

                        z->parent->color = arc_t::color_t::Black;
                        z->parent->parent->color = arc_t::color_t::Red;
                        right_rotate(z->parent->parent);
                    }
                } else {
                    auto y = z->parent->parent->left;

                    if (y->color == arc_t::color_t::Red) {
                        z->parent->color = arc_t::color_t::Black;
                        y->color = arc_t::color_t::Black;
                        z->parent->parent->color = arc_t::color_t::Red;
                        z = z->parent->parent;
                    } else {
                        if (z == z->parent->left) {
                            z = z->parent;
                            right_rotate(z);
                        }

                        z->parent->color = arc_t::color_t::Black;
                        z->parent->parent->color = arc_t::color_t::Red;
                        left_rotate(z->parent->parent);
                    }
                }
            }

            _root->color = arc_t::color_t::Black;
        }

        void remove_fixup(arc_t* x) {
            while (x != _root && x->color == arc_t::color_t::Black) {
                if (x == x->parent->left) {
                    auto w = x->parent->right;

                    if (w->color == arc_t::color_t::Red) {
                        w->color = arc_t::color_t::Black;
                        x->parent->color = arc_t::color_t::Red;
                        left_rotate(x->parent);
                        w = x->parent->right;
                    }

                    if (w->left->color == arc_t::color_t::Black && w->right->color == arc_t::color_t::Black) {
                        w->color = arc_t::color_t::Red;
                        x = x->parent;
                    } else {
                        if (w->right->color == arc_t::color_t::Black) {
                            w->left->color = arc_t::color_t::Black;
                            w->color = arc_t::color_t::Red;
                            right_rotate(w);
                            w = x->parent->right;
                        }

                        w->color = x->parent->color;
                        x->parent->color = arc_t::color_t::Black;
                        w->right->color = arc_t::color_t::Black;
                        left_rotate(x->parent);
                        x = _root;
                    }
                } else {
                    auto w = x->parent->left;

                    if (w->color == arc_t::color_t::Red) {
                        w->color = arc_t::color_t::Black;
                        x->parent->color = arc_t::color_t::Red;
                        right_rotate(x->parent);
                        w = x->parent->left;
                    }

                    if (w->left->color == arc_t::color_t::Black && w->right->color == arc_t::color_t::Black) {
                        w->color = arc_t::color_t::Red;
                        x = x->parent;
                    } else {
                        if (w->left->color == arc_t::color_t::Black) {
                            w->right->color = arc_t::color_t::Black;
                            w->color = arc_t::color_t::Red;
                            left_rotate(w);
                            w = x->parent->left;
                        }

                        w->color = x->parent->color;
                        x->parent->color = arc_t::color_t::Black;
                        w->left->color = arc_t::color_t::Black;
                        right_rotate(x->parent);
                        x = _root;
                    }
                }
            }

            x->color = arc_t::color_t::Black;
        }

        void left_rotate(arc_t* x) {
            auto y = x->right;
            x->right = y->left;
            if (!is_nil(y->left))
                y->left->parent = x;
            y->parent = x->parent;
            if (is_nil(x->parent))
                _root = y;
            else if (x->parent->left == x)
                x->parent->left = y;
            else
                x->parent->right = y;
            y->left = x;
            x->parent = y;
        }

        void right_rotate(arc_t* y) {
            auto x = y->left;
            y->left = x->right;
            if (!is_nil(x->right))
                x->right->parent = y;
            x->parent = y->parent;
            if (is_nil(y->parent))
                _root = x;
            else if (y->parent->left == y)
                y->parent->left = x;
            else
                y->parent->right = x;
            x->right = y;
            y->parent = x;
        }

        arc_t* minimum(arc_t* arc) const {
            while (!is_nil(arc->left))
                arc = arc->left;
            return arc;
        }
    };
}

#endif //DVORONOI_ARC_TREE_HPP
