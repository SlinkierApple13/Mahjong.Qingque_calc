#ifndef QINGQUE_DATA_HPP
#define QINGQUE_DATA_HPP

#include <bitset>
#include <unordered_map>
#include <stdint.h>

namespace qingque {

    constexpr unsigned code_size = 96;

    struct w_data {
        uint64_t total_weight;
        std::unordered_map<std::bitset<code_size>, uint64_t> weights;
    };
    
}

#endif