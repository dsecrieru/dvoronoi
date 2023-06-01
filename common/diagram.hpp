//
// Created by Daniel Secrieru on 30/04/2023.
//

#ifndef DVORONOI_COMMON_DIAGRAM_HPP
#define DVORONOI_COMMON_DIAGRAM_HPP

namespace dvoronoi {

    template<typename point_t>
    struct vertex {
        point_t point{};
    };

    template<typename point_t>
    struct half_edge {
        std::optional<std::size_t> orig{};
        std::optional<std::size_t> dest{};
    };

    template<typename point_t>
    struct diagram_t {
        std::vector<vertex<point_t>> vertices{};
        std::vector<half_edge<point_t>> half_edges{};
    };

} // namespace dvoronoi

#endif //DVORONOI_COMMON_DIAGRAM_HPP
