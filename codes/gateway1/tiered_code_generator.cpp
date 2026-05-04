#include "tiered_code_generator.h"

#include <algorithm>
#include <array>

#include "gold_code_generator.h"
#include "weil_code_generator.h"

namespace lunanet::gateway1 {

std::vector<uint8_t> GenerateAfsI(int prn, const SpreadingSpecTables& tables, std::string* error_message) {
    return GenerateGoldCode(prn, tables, error_message);
}

std::vector<uint8_t> GenerateAfsQ(int prn,
                                  const SpreadingSpecTables& tables,
                                  size_t max_chips,
                                  std::string* error_message) {
    std::string local_error;
    std::string* error_target = (error_message != nullptr) ? error_message : &local_error;

    const std::vector<uint8_t> primary = GenerateWeilPrimary(prn, tables, error_target);
    if (primary.empty()) {
        return {};
    }

    const std::vector<uint8_t> tertiary = GenerateWeilTertiary(prn, tables, error_target);
    if (tertiary.empty()) {
        return {};
    }

    const int secondary_index = tables.SecondaryIndexForPrn(prn);
    if (secondary_index < 0 || secondary_index > 3) {
        *error_target = "No valid secondary code assignment for PRN " + std::to_string(prn);
        return {};
    }

    const std::array<uint8_t, 4>& secondary = tables.secondary_codes[secondary_index];
    const size_t primary_length = primary.size();
    const size_t secondary_length = secondary.size();
    const size_t tertiary_length = tertiary.size();
    const size_t primary_secondary_period = primary_length * secondary_length;
    const size_t full_length = primary_secondary_period * tertiary_length;

    const size_t requested = (max_chips == 0) ? full_length : std::min(max_chips, full_length);
    std::vector<uint8_t> afs_q;
    afs_q.reserve(requested);

    for (size_t i = 0; i < requested; ++i) {
        const uint8_t p = primary[i % primary_length];
        const uint8_t s = secondary[(i / primary_length) % secondary_length];
        const uint8_t t = tertiary[(i / primary_secondary_period) % tertiary_length];
        afs_q.push_back(static_cast<uint8_t>(p ^ s ^ t));
    }

    return afs_q;
}

}  // namespace lunanet::gateway1
