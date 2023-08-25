//
// Created by Daniel Secrieru on 01/08/2023.
//

#ifndef EMERGENT_DATA_HPP
#define EMERGENT_DATA_HPP

namespace dvoronoi::data {

    template<typename point_t>
    struct face_t;

    template<typename point_t>
    struct site_t {
        std::size_t index;
        point_t point{};
        face_t<point_t>* face = nullptr;

        explicit site_t(std::size_t i, auto x, auto y) : index(i), point{x, y} {}
    };

    template<typename point_t>
    struct vertex_t {
        point_t point{};

        vertex_t(auto x, auto y) : point{x, y} {}
    };

    template<typename point_t>
    struct half_edge_t {
        vertex_t<point_t>* orig = nullptr;
        vertex_t<point_t>* dest = nullptr;
        half_edge_t* twin = nullptr;
        face_t<point_t>* face = nullptr;

        half_edge_t* prev = nullptr;
        half_edge_t* next = nullptr;
    };

    template<typename point_t>
    struct face_t {
        site_t<point_t>* site = nullptr;
        half_edge_t<point_t>* half_edge = nullptr;
    };

    template<typename point_t>
    static bool contains(const face_t<point_t>& face, const point_t& p, bool closed_boundary = true) {
        auto count = 0;
        auto he = face.half_edge;
        do {
            if ((he->orig->point.y <= p.y && he->dest->point.y > p.y) ||
                (he->orig->point.y > p.y && he->dest->point.y <= p.y)) {

                double vt = (p.y - he->orig->point.y) / (he->dest->point.y - he->orig->point.y);

                if (p.x > he->orig->point.x + vt * (he->dest->point.x - he->orig->point.x)) {
                    ++count;
                } else if (p.x == he->orig->point.x + vt * (he->dest->point.x - he->orig->point.x)) {
                    return closed_boundary;
                }
            }

            he = he->next;
        } while (he != face.half_edge);

        return count % 2 == 1;
    }
}

#endif //EMERGENT_DATA_HPP
