//
// Created by Daniel Secrieru on 24/07/2023.
//

#ifndef DVORONOI_BOX_HPP
#define DVORONOI_BOX_HPP

#include <array>

#include "scalar.hpp"
#include "point.hpp"
#include "util.hpp"

namespace dvoronoi {

    using namespace util;

    namespace box_side {
        constexpr std::size_t Left = 0;
        constexpr std::size_t Bottom = 1;
        constexpr std::size_t Right = 2;
        constexpr std::size_t Top = 3;
    }

    struct intersection_t {
        std::size_t side;
        _internal::point2_t point;
    };

    struct box_t {
        _internal::scalar_t left = -std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t bottom = -std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t right = std::numeric_limits<_internal::scalar_t>::infinity();
        _internal::scalar_t top = std::numeric_limits<_internal::scalar_t>::infinity();

        [[nodiscard]] bool contains(const _internal::point2_t& p) const {
            return between_eq(p.x, left, right) && between_eq(p.y, bottom, top);
        }

        std::size_t intersections(const _internal::point2_t& orig, const _internal::point2_t& dest,
                                  std::array<intersection_t, 2>& intersections) const {
            auto direction = dest - orig;
            auto t = std::array<_internal::scalar_t, 2>{};
            auto i = std::size_t(0); // index of the current intersection

            // Left
            if (lt(orig.x, left) || lt(dest.x, left)) {
                t[i] = (left - orig.x) / direction.x;
                if (between(t[i], 0.0, 1.0)) {
                    intersections[i].side = box_side::Left;
                    intersections[i].point = orig + t[i] * direction;
                    // Check that the intersection is inside the box
                    if (between_eq(intersections[i].point.y, bottom, top))
                        ++i;
                }
            }

            // Right
            if (gt(orig.x, right) || gt(dest.x, right)) {
                t[i] = (right - orig.x) / direction.x;
                if (between(t[i], 0.0, 1.0)) {
                    intersections[i].side = box_side::Right;
                    intersections[i].point = orig + t[i] * direction;
                    // Check that the intersection is inside the box
                    if (between_eq(intersections[i].point.y, bottom, top))
                        ++i;
                }
            }

            // Bottom
            if (i < 2 && (lt(orig.y, bottom) || lt(dest.y, bottom))) {
                t[i] = (bottom - orig.y) / direction.y;
                if (between(t[i], 0.0, 1.0)) {
                    intersections[i].side = box_side::Bottom;
                    intersections[i].point = orig + t[i] * direction;
                    // Check that the intersection is inside the box
                    if (between_eq(intersections[i].point.x, left, right))
                        ++i;
                }
            }

            // Top
            if (i < 2 && (gt(orig.y, top) || gt(dest.y, top))) {
                t[i] = (top - orig.y) / direction.y;
                if (between(t[i], 0.0, 1.0)) {
                    intersections[i].side = box_side::Top;
                    intersections[i].point = orig + t[i] * direction;
                    // Check that the intersection is inside the box
                    if (between_eq(intersections[i].point.x, left, right))
                        ++i;
                }
            }

            // Sort the intersections from the nearest to the farthest
            if (i == 2 && t[0] > t[1])
                std::swap(intersections[0], intersections[1]);

            return i;
        }
    };

} // namespace dvoronoi::_internal

#endif //DVORONOI_BOX_HPP
