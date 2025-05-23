//
// Created by Daniel Secrieru on 24/07/2023.
//

#ifndef DVORONOI_CONFIG_HPP
#define DVORONOI_CONFIG_HPP

#include <optional>

#include "dvoronoi/common/box.hpp"

namespace dvoronoi::fortune {

    struct config_t {
        std::optional<box_t> bounding_box{};
        bool clip{false};
    };

}

#endif //DVORONOI_CONFIG_HPP
