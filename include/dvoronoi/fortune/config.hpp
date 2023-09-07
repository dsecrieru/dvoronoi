//
// Created by Daniel Secrieru on 24/07/2023.
//

#ifndef DVORONOI_CONFIG_HPP
#define DVORONOI_CONFIG_HPP

#include <optional>

#include "dvoronoi/common/none.hpp"
#include "dvoronoi/common/box.hpp"

namespace dvoronoi::fortune {

    template<typename face_user_data = none, typename half_edge_user_data = none>
    struct config_t {
        typedef face_user_data face_user_data_t;
        typedef half_edge_user_data half_edge_user_data_t;

        std::optional<box_t> bounding_box{};
    };

}

#endif //DVORONOI_CONFIG_HPP
