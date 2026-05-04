#ifndef LUNANET_GATEWAY1_TIERED_CODE_GENERATOR_H
#define LUNANET_GATEWAY1_TIERED_CODE_GENERATOR_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "spreading_config.h"

namespace lunanet::gateway1 {

std::vector<uint8_t> GenerateAfsI(
    int prn,
    const SpreadingSpecTables& tables,
    std::string* error_message);

std::vector<uint8_t> GenerateAfsQ(
    int prn,
    const SpreadingSpecTables& tables,
    size_t max_chips,
    std::string* error_message);

}  // namespace lunanet::gateway1

#endif  // LUNANET_GATEWAY1_TIERED_CODE_GENERATOR_H
