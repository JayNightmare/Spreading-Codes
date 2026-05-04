#ifndef LUNANET_GATEWAY1_WEIL_CODE_GENERATOR_H
#define LUNANET_GATEWAY1_WEIL_CODE_GENERATOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "spreading_config.h"

namespace lunanet::gateway1 {

std::vector<uint8_t> GenerateLegendreSequence(int prime);

std::vector<uint8_t> GenerateWeilPrimary(
    int prn,
    const SpreadingSpecTables& tables,
    std::string* error_message);

std::vector<uint8_t> GenerateWeilTertiary(
    int prn,
    const SpreadingSpecTables& tables,
    std::string* error_message);

}  // namespace lunanet::gateway1

#endif  // LUNANET_GATEWAY1_WEIL_CODE_GENERATOR_H
