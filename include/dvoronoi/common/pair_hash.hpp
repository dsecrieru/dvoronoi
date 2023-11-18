//
// Created by Daniel Secrieru on 17/11/2023.
//

#ifndef DVORONOI_PAIR_HASH_HPP
#define DVORONOI_PAIR_HASH_HPP

namespace dvoronoi {

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &pair) const {
            return std::hash<T1>{}(pair.first) ^ std::hash<T2>{}(pair.second);
        }
    };

} // dvoronoi

#endif //DVORONOI_PAIR_HASH_HPP
