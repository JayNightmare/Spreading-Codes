#ifndef LUNANET_SPREADING_CODES_H
#define LUNANET_SPREADING_CODES_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace lunanet {

constexpr int MAX_PRNS = 210;
constexpr int GOLD_CODE_LENGTH = 2046;
constexpr int WEIL_PRIMARY_PRIME = 10223;
constexpr int WEIL_TERTIARY_PRIME = 1499;
constexpr int WEIL_TERTIARY_LENGTH = 1500;
constexpr int EXPANSION_LENGTH = 7;

void initialize_engine();
const char* hello_moon();
const char* get_version();
const char* get_last_error();

bool load_spreading_code_config(const std::string& config_path, std::string* error_message = nullptr);
const char* get_annex3_gold_path();
const char* get_annex3_weil_primary_path();
const char* get_annex3_weil_tertiary_path();
size_t get_afs_q_max_chips();

std::vector<uint8_t> generate_gold_code(int prn);
std::vector<uint8_t> generate_legendre_sequence(int prime);
std::vector<uint8_t> generate_weil_primary(int prn);
std::vector<uint8_t> generate_weil_tertiary(int prn);
std::vector<uint8_t> generate_afs_i(int prn);
std::vector<uint8_t> generate_afs_q(int prn, size_t max_chips = 0);

std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> generate_all_spreading_codes();

std::string chips_to_hex(const std::vector<uint8_t>& chips, size_t num_chips = 0);
std::string vector_to_hex(const std::vector<uint8_t>& chips, size_t num_chips = 24);
std::vector<uint8_t> hex_to_vector(const std::string& hex, size_t num_chips = 0);

bool load_reference_hex_file(const std::string& path,
                             size_t expected_chips,
                             std::vector<std::vector<uint8_t>>& out,
                             std::string* error_message = nullptr);

}  // namespace lunanet

#endif  // LUNANET_SPREADING_CODES_H
