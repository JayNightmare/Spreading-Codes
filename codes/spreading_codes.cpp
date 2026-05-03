/**
 * LunaNet Gateway 1 - Spreading Code Generator Implementation
 * 
 * This file implements the AFS (AltBOC Forward link Spreading) codes
 * for the LunaNet navigation system.
 */

#include "spreading_codes.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <map>
#include <array>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <cctype>

namespace lunanet {

    namespace {

        struct SpreadingCodeConfig {
            std::vector<int> g2_delays;
            std::vector<int> weil_primary_k;
            std::vector<int> weil_primary_p;
            std::vector<int> weil_tertiary_k;
            std::array<std::array<uint8_t, 4>, 4> secondary_codes{};
            int afs_q_secondary_index = 0;
            size_t afs_q_max_chips = 0;
            std::string annex3_gold_path;
            std::string annex3_weil_primary_path;
            std::string annex3_weil_tertiary_path;
        };

        SpreadingCodeConfig config;
        bool config_loaded = false;

        std::string trim(const std::string& input) {
            size_t start = 0;
            while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))) {
                ++start;
            }
            size_t end = input.size();
            while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
                --end;
            }
            return input.substr(start, end - start);
        }

        std::vector<std::string> split_csv_line(const std::string& line) {
            std::vector<std::string> fields;
            std::string current;
            bool in_quotes = false;

            for (char ch : line) {
                if (ch == '"') {
                    in_quotes = !in_quotes;
                    continue;
                }

                if (ch == ',' && !in_quotes) {
                    fields.push_back(trim(current));
                    current.clear();
                } else {
                    current.push_back(ch);
                }
            }

            fields.push_back(trim(current));
            return fields;
        }

        bool parse_int(const std::string& text, int* out) {
            if (!out) return false;
            const std::string trimmed = trim(text);
            if (trimmed.empty()) return false;
            try {
                size_t idx = 0;
                int value = std::stoi(trimmed, &idx, 10);
                if (idx != trimmed.size()) return false;
                *out = value;
                return true;
            } catch (...) {
                return false;
            }
        }

        bool parse_size(const std::string& text, size_t* out) {
            if (!out) return false;
            const std::string trimmed = trim(text);
            if (trimmed.empty()) return false;
            try {
                size_t idx = 0;
                unsigned long long value = std::stoull(trimmed, &idx, 10);
                if (idx != trimmed.size()) return false;
                *out = static_cast<size_t>(value);
                return true;
            } catch (...) {
                return false;
            }
        }

        int header_index(const std::vector<std::string>& headers, const std::string& key) {
            std::string key_lower = key;
            std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(),
                           [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            for (size_t i = 0; i < headers.size(); ++i) {
                std::string header = headers[i];
                std::transform(header.begin(), header.end(), header.begin(),
                               [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
                if (header == key_lower) return static_cast<int>(i);
            }
            return -1;
        }

        bool read_csv_file(const std::filesystem::path& path,
                           std::vector<std::string>& headers,
                           std::vector<std::vector<std::string>>& rows,
                           std::string* error) {
            std::ifstream file(path);
            if (!file) {
                if (error) {
                    *error = "Failed to open CSV: " + path.string();
                }
                return false;
            }

            headers.clear();
            rows.clear();

            std::string line;
            while (std::getline(file, line)) {
                const std::string trimmed = trim(line);
                if (trimmed.empty() || trimmed[0] == '#') {
                    continue;
                }

                std::vector<std::string> fields = split_csv_line(line);
                if (headers.empty()) {
                    headers = fields;
                    continue;
                }

                if (fields.size() < headers.size()) {
                    fields.resize(headers.size());
                }

                rows.push_back(std::move(fields));
            }

            if (headers.empty()) {
                if (error) {
                    *error = "CSV missing header row: " + path.string();
                }
                return false;
            }

            return true;
        }

        bool load_gold_g2_delays(const std::filesystem::path& path, SpreadingCodeConfig& cfg, std::string* error) {
            std::vector<std::string> headers;
            std::vector<std::vector<std::string>> rows;
            if (!read_csv_file(path, headers, rows, error)) return false;

            const int prn_idx = header_index(headers, "prn");
            int delay_idx = header_index(headers, "g2_delay");
            if (delay_idx < 0) {
                delay_idx = header_index(headers, "g2_delay_chips");
            }

            if (prn_idx < 0 || delay_idx < 0) {
                if (error) {
                    *error = "CSV missing required columns in " + path.string();
                }
                return false;
            }

            cfg.g2_delays.assign(MAX_PRNS, -1);
            for (const auto& row : rows) {
                int prn = 0;
                int delay = 0;
                if (!parse_int(row[prn_idx], &prn) || !parse_int(row[delay_idx], &delay)) {
                    continue;
                }

                if (prn < 1 || prn > MAX_PRNS) {
                    continue;
                }

                cfg.g2_delays[prn - 1] = delay;
            }

            for (int prn = 1; prn <= MAX_PRNS; ++prn) {
                if (cfg.g2_delays[prn - 1] < 0) {
                    if (error) {
                        *error = "Missing G2 delay for PRN " + std::to_string(prn);
                    }
                    return false;
                }
            }

            return true;
        }

        bool load_weil_primary_params(const std::filesystem::path& path, SpreadingCodeConfig& cfg, std::string* error) {
            std::vector<std::string> headers;
            std::vector<std::vector<std::string>> rows;
            if (!read_csv_file(path, headers, rows, error)) return false;

            const int prn_idx = header_index(headers, "prn");
            const int k_idx = header_index(headers, "weil_index_k");
            const int p_idx = header_index(headers, "insertion_index_p");
            if (prn_idx < 0 || k_idx < 0 || p_idx < 0) {
                if (error) {
                    *error = "CSV missing required columns in " + path.string();
                }
                return false;
            }

            cfg.weil_primary_k.assign(MAX_PRNS, -1);
            cfg.weil_primary_p.assign(MAX_PRNS, -1);

            for (const auto& row : rows) {
                int prn = 0;
                int k = 0;
                int p = 0;
                if (!parse_int(row[prn_idx], &prn) || !parse_int(row[k_idx], &k) || !parse_int(row[p_idx], &p)) {
                    continue;
                }

                if (prn < 1 || prn > MAX_PRNS) {
                    continue;
                }

                cfg.weil_primary_k[prn - 1] = k;
                cfg.weil_primary_p[prn - 1] = p;
            }

            for (int prn = 1; prn <= MAX_PRNS; ++prn) {
                if (cfg.weil_primary_k[prn - 1] < 0 || cfg.weil_primary_p[prn - 1] < 0) {
                    if (error) {
                        *error = "Missing Weil primary parameters for PRN " + std::to_string(prn);
                    }
                    return false;
                }
            }

            return true;
        }

        bool load_weil_tertiary_params(const std::filesystem::path& path, SpreadingCodeConfig& cfg, std::string* error) {
            std::vector<std::string> headers;
            std::vector<std::vector<std::string>> rows;
            if (!read_csv_file(path, headers, rows, error)) return false;

            const int prn_idx = header_index(headers, "prn");
            const int k_idx = header_index(headers, "weil_index_k");
            if (prn_idx < 0 || k_idx < 0) {
                if (error) {
                    *error = "CSV missing required columns in " + path.string();
                }
                return false;
            }

            cfg.weil_tertiary_k.assign(MAX_PRNS, -1);

            for (const auto& row : rows) {
                int prn = 0;
                int k = 0;
                if (!parse_int(row[prn_idx], &prn) || !parse_int(row[k_idx], &k)) {
                    continue;
                }

                if (prn < 1 || prn > MAX_PRNS) {
                    continue;
                }

                cfg.weil_tertiary_k[prn - 1] = k;
            }

            for (int prn = 1; prn <= MAX_PRNS; ++prn) {
                if (cfg.weil_tertiary_k[prn - 1] < 0) {
                    if (error) {
                        *error = "Missing Weil tertiary parameters for PRN " + std::to_string(prn);
                    }
                    return false;
                }
            }

            return true;
        }

        int secondary_index_from_id(const std::string& id) {
            if (id.size() < 2) return -1;
            if (id[0] != 'S' && id[0] != 's') return -1;
            const char index_char = id[1];
            if (index_char < '0' || index_char > '3') return -1;
            return index_char - '0';
        }

        bool load_secondary_codes(const std::filesystem::path& path, SpreadingCodeConfig& cfg, std::string* error) {
            std::vector<std::string> headers;
            std::vector<std::vector<std::string>> rows;
            if (!read_csv_file(path, headers, rows, error)) return false;

            const int id_idx = header_index(headers, "id");
            const int chip0_idx = header_index(headers, "chip0");
            const int chip1_idx = header_index(headers, "chip1");
            const int chip2_idx = header_index(headers, "chip2");
            const int chip3_idx = header_index(headers, "chip3");
            if (id_idx < 0 || chip0_idx < 0 || chip1_idx < 0 || chip2_idx < 0 || chip3_idx < 0) {
                if (error) {
                    *error = "CSV missing required columns in " + path.string();
                }
                return false;
            }

            std::array<bool, 4> filled{};
            for (const auto& row : rows) {
                const std::string id = trim(row[id_idx]);
                const int index = secondary_index_from_id(id);
                if (index < 0) continue;

                int chip0 = 0;
                int chip1 = 0;
                int chip2 = 0;
                int chip3 = 0;
                if (!parse_int(row[chip0_idx], &chip0) || !parse_int(row[chip1_idx], &chip1) ||
                    !parse_int(row[chip2_idx], &chip2) || !parse_int(row[chip3_idx], &chip3)) {
                    continue;
                }

                cfg.secondary_codes[index] = {
                    static_cast<uint8_t>(chip0 & 1),
                    static_cast<uint8_t>(chip1 & 1),
                    static_cast<uint8_t>(chip2 & 1),
                    static_cast<uint8_t>(chip3 & 1)
                };
                filled[index] = true;
            }

            for (int i = 0; i < 4; ++i) {
                if (!filled[i]) {
                    if (error) {
                        *error = "Missing secondary code S" + std::to_string(i) + " in " + path.string();
                    }
                    return false;
                }
            }

            return true;
        }

        bool load_spec_tables(const std::filesystem::path& dir, SpreadingCodeConfig& cfg, std::string* error) {
            if (!load_gold_g2_delays(dir / "appendix_c_gold_g2_delays.csv", cfg, error)) return false;
            if (!load_weil_primary_params(dir / "appendix_d_weil_primary_params.csv", cfg, error)) return false;
            if (!load_weil_tertiary_params(dir / "appendix_e_weil_tertiary_params.csv", cfg, error)) return false;
            if (!load_secondary_codes(dir / "table_10_secondary_codes.csv", cfg, error)) return false;
            return true;
        }

        bool parse_key_value_file(const std::filesystem::path& path,
                                  std::unordered_map<std::string, std::string>& values,
                                  std::string* error) {
            std::ifstream file(path);
            if (!file) {
                if (error) {
                    *error = "Failed to open config file: " + path.string();
                }
                return false;
            }

            std::string line;
            while (std::getline(file, line)) {
                std::string trimmed = trim(line);
                if (trimmed.empty() || trimmed[0] == '#') continue;
                const size_t eq = trimmed.find('=');
                if (eq == std::string::npos) continue;
                std::string key = trim(trimmed.substr(0, eq));
                std::string value = trim(trimmed.substr(eq + 1));
                if (!key.empty()) {
                    values[key] = value;
                }
            }

            return true;
        }

        std::filesystem::path resolve_path(const std::filesystem::path& base_dir, const std::string& value) {
            if (value.empty()) return {};
            std::filesystem::path path(value);
            if (path.is_absolute()) return path;
            return base_dir / path;
        }

        std::string get_value(const std::unordered_map<std::string, std::string>& values,
                              const std::string& key,
                              const std::string& fallback) {
            auto it = values.find(key);
            return (it == values.end()) ? fallback : it->second;
        }

    } // namespace

    // Static variables for error handling
    static char last_error[256] = "";
    static const char* version = "1.0.0";

    void initialize_engine() {
        // Initialize any required resources
        std::memset(last_error, 0, sizeof(last_error));
    }

    const char* hello_moon() {
        return "Hello Moon";
    }

    const char* get_last_error() {
        return last_error;
    }

    const char* get_version() {
        return version;
    }

    std::string chips_to_hex(const std::vector<uint8_t>& chips, size_t num_chips) {
        const size_t chip_count = (num_chips == 0) ? chips.size() : std::min(num_chips, chips.size());
        std::string hex_str;
        hex_str.reserve((chip_count + 3) / 4);

        uint8_t current_val = 0;
        for (size_t i = 0; i < chip_count; ++i) {
            current_val = static_cast<uint8_t>((current_val << 1) | (chips[i] & 1u));
            if ((i + 1) % 4 == 0) {
                hex_str += "0123456789ABCDEF"[current_val & 0x0F];
                current_val = 0;
            }
        }

        const size_t remainder = chip_count % 4;
        if (remainder != 0) {
            current_val = static_cast<uint8_t>(current_val << (4 - remainder));
            hex_str += "0123456789ABCDEF"[current_val & 0x0F];
        }

        return hex_str;
    }

    // Helper function to set error message
    static void set_error(const char* msg) {
        std::strncpy(last_error, msg, sizeof(last_error) - 1);
        last_error[sizeof(last_error) - 1] = '\0';
    }

    bool load_spreading_code_config(const std::string& config_path, std::string* error_message) {
        std::string local_error;
        std::string* error_target = error_message ? error_message : &local_error;
        config_loaded = false;

        try {
            std::unordered_map<std::string, std::string> values;
            if (!parse_key_value_file(config_path, values, error_target)) {
                set_error(error_target->c_str());
                return false;
            }

            SpreadingCodeConfig next_config;
            const std::filesystem::path base_dir = std::filesystem::absolute(config_path).parent_path();
            const std::string spec_tables_dir = get_value(values, "spec_tables_dir", "docs/spec_tables");
            const std::filesystem::path spec_dir = resolve_path(base_dir, spec_tables_dir);

            if (!load_spec_tables(spec_dir, next_config, error_target)) {
                set_error(error_target->c_str());
                return false;
            }

            const std::string annex_dir_value = get_value(values, "annex3_dir", "");
            std::filesystem::path annex_base = base_dir;
            if (!annex_dir_value.empty()) {
                annex_base = resolve_path(base_dir, annex_dir_value);
            }

            const std::string gold_file = get_value(values, "annex3_gold", "GoldCode2046hex210prns.txt");
            const std::string primary_file = get_value(values, "annex3_weil_primary", "l1cp_hex210prns.txt");
            const std::string tertiary_file = get_value(values, "annex3_weil_tertiary", "Weil1500hex210prns.txt");

            next_config.annex3_gold_path = resolve_path(annex_base, gold_file).string();
            next_config.annex3_weil_primary_path = resolve_path(annex_base, primary_file).string();
            next_config.annex3_weil_tertiary_path = resolve_path(annex_base, tertiary_file).string();

            const std::string secondary_id = get_value(values, "afs_q_secondary_code_id", "S0");
            const int secondary_index = secondary_index_from_id(secondary_id);
            if (secondary_index < 0) {
                *error_target = "Invalid afs_q_secondary_code_id: " + secondary_id;
                set_error(error_target->c_str());
                return false;
            }
            next_config.afs_q_secondary_index = secondary_index;

            const std::string max_chips_value = get_value(values, "afs_q_max_chips", "");
            if (!max_chips_value.empty()) {
                size_t max_chips = 0;
                if (!parse_size(max_chips_value, &max_chips)) {
                    *error_target = "Invalid afs_q_max_chips value";
                    set_error(error_target->c_str());
                    return false;
                }
                next_config.afs_q_max_chips = max_chips;
            }

            config = std::move(next_config);
            config_loaded = true;
            return true;
        } catch (const std::exception& ex) {
            *error_target = ex.what();
            set_error(error_target->c_str());
            return false;
        }
    }

    const char* get_annex3_gold_path() {
        return config_loaded ? config.annex3_gold_path.c_str() : "";
    }

    const char* get_annex3_weil_primary_path() {
        return config_loaded ? config.annex3_weil_primary_path.c_str() : "";
    }

    const char* get_annex3_weil_tertiary_path() {
        return config_loaded ? config.annex3_weil_tertiary_path.c_str() : "";
    }

    size_t get_afs_q_max_chips() {
        return config_loaded ? config.afs_q_max_chips : 0;
    }

    static bool write_hex_file(const std::filesystem::path& path,
                               const std::vector<std::pair<int, std::string>>& rows,
                               std::string* error_message) {
        std::ofstream out(path, std::ios::binary);
        if (!out) {
            if (error_message) {
                *error_message = "Failed to open output file: " + path.string();
            }
            return false;
        }

        for (const auto& row : rows) {
            out << row.first << ',' << row.second << '\n';
        }

        if (!out.good()) {
            if (error_message) {
                *error_message = "Failed to write output file: " + path.string();
            }
            return false;
        }

        return true;
    }

    bool export_reference_hex_files(const std::string& output_directory, std::string* error_message) {
        try {
            if (!config_loaded) {
                if (error_message) {
                    *error_message = "Spreading code config not loaded";
                }
                set_error("Spreading code config not loaded");
                return false;
            }

            const std::filesystem::path out_dir(output_directory);
            std::filesystem::create_directories(out_dir);

            std::vector<std::pair<int, std::string>> gold_rows;
            std::vector<std::pair<int, std::string>> weil_primary_rows;
            std::vector<std::pair<int, std::string>> weil_tertiary_rows;
            gold_rows.reserve(MAX_PRNS);
            weil_primary_rows.reserve(MAX_PRNS);
            weil_tertiary_rows.reserve(MAX_PRNS);

            for (int prn = 1; prn <= MAX_PRNS; ++prn) {
                gold_rows.emplace_back(prn, chips_to_hex(generate_afs_i(prn)));
                weil_primary_rows.emplace_back(prn, chips_to_hex(generate_weil_primary(prn)));
                weil_tertiary_rows.emplace_back(prn, chips_to_hex(generate_weil_tertiary(prn)));
            }

            if (!write_hex_file(out_dir / "GoldCode2046hex210prns.txt", gold_rows, error_message)) return false;
            if (!write_hex_file(out_dir / "l1cp_hex210prns.txt", weil_primary_rows, error_message)) return false;
            if (!write_hex_file(out_dir / "Weil1500hex210prns.txt", weil_tertiary_rows, error_message)) return false;
            return true;
        } catch (const std::exception& ex) {
            if (error_message) {
                *error_message = ex.what();
            }
            return false;
        }
    }

    static bool diff_single_file(const std::filesystem::path& reference_path,
                                 const std::filesystem::path& generated_path,
                                 std::ostringstream& report) {
        std::ifstream ref(reference_path, std::ios::binary);
        std::ifstream gen(generated_path, std::ios::binary);
        if (!ref || !gen) {
            report << "Missing file: " << reference_path.string() << " or " << generated_path.string() << '\n';
            return false;
        }

        std::string ref_line;
        std::string gen_line;
        size_t line_no = 0;
        bool matched = true;
        while (true) {
            const bool ref_ok = static_cast<bool>(std::getline(ref, ref_line));
            const bool gen_ok = static_cast<bool>(std::getline(gen, gen_line));
            if (!ref_ok && !gen_ok) break;
            ++line_no;
            if (ref_ok != gen_ok || ref_line != gen_line) {
                matched = false;
                report << "Mismatch at line " << line_no << " in " << reference_path.filename().string() << '\n';
                if (!ref_ok) report << "  reference: <EOF>\n"; else report << "  reference: " << ref_line << '\n';
                if (!gen_ok) report << "  generated: <EOF>\n"; else report << "  generated: " << gen_line << '\n';
                break;
            }
        }

        if (matched) {
            report << "Match: " << reference_path.filename().string() << '\n';
        }
        return matched;
    }

    bool diff_reference_hex_files(const std::string& reference_directory, const std::string& generated_directory, std::string* report) {
        try {
            std::ostringstream summary;
            bool all_match = true;
            const std::filesystem::path ref_dir(reference_directory);
            const std::filesystem::path gen_dir(generated_directory);

            all_match &= diff_single_file(ref_dir / "GoldCode2046hex210prns.txt", gen_dir / "GoldCode2046hex210prns.txt", summary);
            all_match &= diff_single_file(ref_dir / "l1cp_hex210prns.txt", gen_dir / "l1cp_hex210prns.txt", summary);
            all_match &= diff_single_file(ref_dir / "Weil1500hex210prns.txt", gen_dir / "Weil1500hex210prns.txt", summary);

            if (report) {
                *report = summary.str();
            }
            return all_match;
        } catch (const std::exception& ex) {
            if (report) {
                *report = ex.what();
            }
            return false;
        }
    }

    // Linear Feedback Shift Register (LFSR) implementation for 11-bit and other lengths
    class LFSR {
    private:
        uint16_t state;
        uint16_t feedback_mask;
        int length;
    
    public:
        LFSR(uint16_t initial_state, uint16_t feedback_mask, int reg_length = 11)
            : state(initial_state), feedback_mask(feedback_mask), length(reg_length) {}
    
        // Generate next bit and update state
        uint8_t next_bit() {
            uint8_t output = (state >> (length - 1)) & 1;

            uint16_t masked_state = state & feedback_mask;
            int feedback = 0;
            for (int i = 0; i < length; i++) {
                feedback ^= (masked_state >> i) & 1;
            }

            state = ((state << 1) & ((1 << length) - 1)) | feedback;

            return output;
        }
    
        // Reset to initial state
        void reset(uint16_t initial_state) {
            state = initial_state;
        }
    
        // Get current state
        uint16_t get_state() const {
            return state;
        }
    };

    // Legendre symbol computation for Weil code generation
    static int legendre_symbol(int a, int p) {
        if (a == 0) return 0;
        if (p == 2) return a & 1; // For p=2, return a mod 2
    
        // Euler's criterion: a^((p-1)/2) mod p
        long long result = 1;
        long long base = a % p;
        long long exponent = (p - 1) / 2;
    
        while (exponent > 0) {
            if (exponent & 1) {
                result = (result * base) % p;
            }
            base = (base * base) % p;
            exponent >>= 1;
        }
    
        if (result == p - 1) return -1;
        return result;
    }

    // Generate Gold code for a given PRN (11-bit LFSR, 2046-chip output)
    // Per LSIS-AFS Appendix C: uses 11-bit generators with PRN-specific G2 delay applied at runtime.
    std::vector<uint8_t> generate_gold_code(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }

        if (!config_loaded || config.g2_delays.size() != static_cast<size_t>(MAX_PRNS)) {
            set_error("Spreading code config not loaded");
            return {};
        }
    
        // G1 uses fixed all-1s initialization for 11 bits
        LFSR g1(G1_INIT, G1_TAP_FEEDBACK, LFSR_LENGTH);
    
        // G2 uses the all-1s initialization, pre-advanced by the PRN-specific delay.
        const int g2_delay = config.g2_delays[prn - 1];
        if (g2_delay < 0) {
            set_error("Missing G2 delay for PRN");
            return {};
        }
        LFSR g2(G1_INIT, G2_TAP_FEEDBACK, LFSR_LENGTH);
        for (int i = 0; i < g2_delay; ++i) {
            g2.next_bit();
        }
    
        std::vector<uint8_t> gold_code;
        gold_code.reserve(GOLD_CODE_LENGTH);
    
        // Generate 2046 chips (2^11 - 1 shortened cycle)
        for (int i = 0; i < GOLD_CODE_LENGTH; i++) {
            uint8_t g1_bit = g1.next_bit();
            uint8_t g2_bit = g2.next_bit();
            gold_code.push_back(g1_bit ^ g2_bit);
        }
    
        return gold_code;
    }

    // Generate Legendre sequence for a given prime
    std::vector<uint8_t> generate_legendre_sequence(int prime) {
        std::vector<uint8_t> legendre;
        legendre.reserve(prime);
    
        for (int n = 0; n < prime; n++) {
            int symbol = legendre_symbol(n, prime);
            // Convert Legendre symbol to binary: +1 -> 1, (0 or -1) -> 0
            legendre.push_back((symbol == 1) ? 1 : 0);
        }
    
        return legendre;
    }

    // Generate PRN-specific Weil primary code (Appendix D)
    // W(t;k) = L(t) XOR L((t+k) mod 10223), with expansion [0,1,1,0,1,0,0] inserted at index p
    std::vector<uint8_t> generate_weil_primary(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }

        if (!config_loaded || config.weil_primary_k.size() != static_cast<size_t>(MAX_PRNS)) {
            set_error("Spreading code config not loaded");
            return {};
        }
    
        // Get PRN-specific Weil parameters
        int k = config.weil_primary_k[prn - 1];
        int p = config.weil_primary_p[prn - 1];
        if (k < 0 || k >= WEIL_PRIMARY_PRIME || p < 0 || p >= WEIL_PRIMARY_PRIME) {
            set_error("Invalid Weil primary parameters for PRN");
            return {};
        }
    
        // Generate Legendre sequence L(t) for prime 10223
        std::vector<uint8_t> legendre = generate_legendre_sequence(WEIL_PRIMARY_PRIME);
    
        // Compute Weil code: W(t;k) = L(t) XOR L((t+k) mod 10223)
        std::vector<uint8_t> weil;
        weil.reserve(WEIL_PRIMARY_PRIME + EXPANSION_LENGTH);
    
        for (int t = 0; t < WEIL_PRIMARY_PRIME; t++) {
            uint8_t bit = legendre[t] ^ legendre[(t + k) % WEIL_PRIMARY_PRIME];
        
            // Insert expansion at index p
            if (t == p) {
                weil.push_back(bit);
                // Insert 7-bit expansion: [0, 1, 1, 0, 1, 0, 0]
                weil.push_back(0);
                weil.push_back(1);
                weil.push_back(1);
                weil.push_back(0);
                weil.push_back(1);
                weil.push_back(0);
                weil.push_back(0);
            } else {
                weil.push_back(bit);
            }
        }
    
        return weil;
    }

    // Generate PRN-specific Weil tertiary code (Appendix E)
    // W(t;k) = L(t) XOR L((t+k) mod 1499), with 0 appended at end
    std::vector<uint8_t> generate_weil_tertiary(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }

        if (!config_loaded || config.weil_tertiary_k.size() != static_cast<size_t>(MAX_PRNS)) {
            set_error("Spreading code config not loaded");
            return {};
        }
    
        // Get PRN-specific Weil parameter
        int k = config.weil_tertiary_k[prn - 1];
        if (k < 0 || k >= WEIL_TERTIARY_PRIME) {
            set_error("Invalid Weil tertiary parameters for PRN");
            return {};
        }
    
        // Generate Legendre sequence L(t) for prime 1499
        std::vector<uint8_t> legendre = generate_legendre_sequence(WEIL_TERTIARY_PRIME);
    
        // Compute Weil code: W(t;k) = L(t) XOR L((t+k) mod 1499)
        std::vector<uint8_t> weil;
        weil.reserve(WEIL_TERTIARY_LENGTH);
    
        for (int t = 0; t < WEIL_TERTIARY_PRIME; t++) {
            uint8_t bit = legendre[t] ^ legendre[(t + k) % WEIL_TERTIARY_PRIME];
            weil.push_back(bit);
        }
    
        // Append final 0 per spec
        weil.push_back(0);
    
        return weil;
    }

    // Generate the complete AFS-I data channel (Gold codes, 2046 chips)
    std::vector<uint8_t> generate_afs_i(int prn) {
        return generate_gold_code(prn);
    }

    // Generate the complete AFS-Q pilot channel (tiered XOR per spec)
    std::vector<uint8_t> generate_afs_q(int prn, size_t max_chips) {
        if (!config_loaded) {
            set_error("Spreading code config not loaded");
            return {};
        }

        std::vector<uint8_t> primary = generate_weil_primary(prn);
        std::vector<uint8_t> tertiary = generate_weil_tertiary(prn);
        if (primary.empty() || tertiary.empty()) {
            return {};
        }

        const std::array<uint8_t, 4>& secondary = config.secondary_codes[config.afs_q_secondary_index];
        const size_t primary_len = primary.size();
        const size_t secondary_len = secondary.size();
        const size_t tertiary_len = tertiary.size();
        if (primary_len == 0 || tertiary_len == 0) {
            set_error("AFS-Q requires non-empty primary and tertiary codes");
            return {};
        }
        const size_t tiered_period = primary_len * secondary_len;
        const size_t total_len = tiered_period * tertiary_len;
        const size_t limit = (max_chips > 0) ? std::min(max_chips, total_len) : total_len;

        std::vector<uint8_t> afs_q;
        afs_q.reserve(limit);

        for (size_t i = 0; i < limit; ++i) {
            const uint8_t p_chip = primary[i % primary_len];
            const uint8_t s_chip = secondary[(i / primary_len) % secondary_len];
            const uint8_t t_chip = tertiary[i / tiered_period];
            afs_q.push_back(static_cast<uint8_t>(p_chip ^ s_chip ^ t_chip));
        }

        return afs_q;
    }

    // Generate all spreading codes for all 210 PRNs (Phase 1B batch generator)
    std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> generate_all_spreading_codes() {
        std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> all_codes;
        const size_t afs_q_max = get_afs_q_max_chips();

        for (int prn = 1; prn <= MAX_PRNS; ++prn) {
            std::vector<uint8_t> afs_i = generate_afs_i(prn);
            std::vector<uint8_t> afs_q = generate_afs_q(prn, afs_q_max);
            all_codes[prn] = std::make_pair(afs_i, afs_q);
        }
    
        return all_codes;
    }
} // namespace lunanet 