#ifndef LUNANET_GATEWAY1_SPREADING_CONFIG_H
#define LUNANET_GATEWAY1_SPREADING_CONFIG_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway1 {

constexpr int kMaxPrns = 210;
constexpr int kGoldCodeLength = 2046;  // 2^11 - 1 shortened cycle.
constexpr int kWeilPrimaryPrime = 10223;
constexpr int kWeilTertiaryPrime = 1499;
constexpr int kWeilTertiaryLength = 1500;
constexpr int kExpansionLength = 7;

struct Annex3Paths {
    std::string gold;
    std::string weil_primary;
    std::string weil_tertiary;
};

struct SpreadingSpecTables {
    std::vector<int> g2_delays;
    std::vector<int> weil_primary_k;
    std::vector<int> weil_primary_insert_index;
    std::vector<int> weil_tertiary_k;
    std::array<std::array<uint8_t, 4>, 4> secondary_codes{};
    std::vector<int> secondary_code_by_prn;
    size_t afs_q_max_chips = 0;

    bool IsReady() const;
    int SecondaryIndexForPrn(int prn) const;
};

int SecondaryCodeIndexFromId(const std::string& id);

bool LoadSpreadingConfig(
    const std::string& config_path,
    SpreadingSpecTables* out_tables,
    Annex3Paths* out_annex3_paths,
    std::string* error_message);

}  // namespace lunanet::gateway1

#endif  // LUNANET_GATEWAY1_SPREADING_CONFIG_H
