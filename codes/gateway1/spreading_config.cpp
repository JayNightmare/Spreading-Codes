#include "spreading_config.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace lunanet::gateway1 {

namespace {

struct CsvData {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
};

std::string Trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(start, end - start);
}

std::vector<std::string> SplitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;

    for (const char ch : line) {
        if (ch == '"') {
            in_quotes = !in_quotes;
            continue;
        }

        if (ch == ',' && !in_quotes) {
            fields.push_back(Trim(current));
            current.clear();
            continue;
        }

        current.push_back(ch);
    }

    fields.push_back(Trim(current));
    return fields;
}

bool ParseIntStrict(const std::string& text, int* out) {
    if (out == nullptr) {
        return false;
    }

    const std::string trimmed = Trim(text);
    if (trimmed.empty()) {
        return false;
    }

    char* end_ptr = nullptr;
    const long parsed = std::strtol(trimmed.c_str(), &end_ptr, 10);
    if (end_ptr == trimmed.c_str() || *end_ptr != '\0') {
        return false;
    }

    *out = static_cast<int>(parsed);
    return true;
}

bool ParseSizeStrict(const std::string& text, size_t* out) {
    if (out == nullptr) {
        return false;
    }

    const std::string trimmed = Trim(text);
    if (trimmed.empty()) {
        return false;
    }

    char* end_ptr = nullptr;
    const unsigned long long parsed = std::strtoull(trimmed.c_str(), &end_ptr, 10);
    if (end_ptr == trimmed.c_str() || *end_ptr != '\0') {
        return false;
    }

    *out = static_cast<size_t>(parsed);
    return true;
}

int HeaderIndex(const std::vector<std::string>& headers, const std::string& key) {
    std::string key_lower = key;
    std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (size_t i = 0; i < headers.size(); ++i) {
        std::string header = headers[i];
        std::transform(header.begin(), header.end(), header.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (header == key_lower) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool ReadCsvFile(const std::filesystem::path& path, CsvData* out, std::string* error_message) {
    if (out == nullptr) {
        if (error_message != nullptr) {
            *error_message = "ReadCsvFile called with null output";
        }
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        if (error_message != nullptr) {
            *error_message = "Failed to open CSV file: " + path.string();
        }
        return false;
    }

    out->headers.clear();
    out->rows.clear();

    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        std::vector<std::string> fields = SplitCsvLine(line);
        if (out->headers.empty()) {
            out->headers = std::move(fields);
            continue;
        }

        if (fields.size() < out->headers.size()) {
            fields.resize(out->headers.size());
        }
        out->rows.push_back(std::move(fields));
    }

    if (out->headers.empty()) {
        if (error_message != nullptr) {
            *error_message = "CSV has no header row: " + path.string();
        }
        return false;
    }

    return true;
}

bool ParseKeyValueFile(const std::filesystem::path& path,
                       std::unordered_map<std::string, std::string>* out,
                       std::string* error_message) {
    if (out == nullptr) {
        if (error_message != nullptr) {
            *error_message = "ParseKeyValueFile called with null output";
        }
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        if (error_message != nullptr) {
            *error_message = "Failed to open config file: " + path.string();
        }
        return false;
    }

    out->clear();

    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        const size_t equal_position = trimmed.find('=');
        if (equal_position == std::string::npos) {
            continue;
        }

        const std::string key = Trim(trimmed.substr(0, equal_position));
        const std::string value = Trim(trimmed.substr(equal_position + 1));
        if (!key.empty()) {
            (*out)[key] = value;
        }
    }

    return true;
}

std::string ValueOrDefault(const std::unordered_map<std::string, std::string>& values,
                           const std::string& key,
                           const std::string& fallback) {
    const auto it = values.find(key);
    return (it == values.end()) ? fallback : it->second;
}

std::filesystem::path ResolvePath(const std::filesystem::path& base_dir, const std::string& value) {
    std::filesystem::path path(value);
    if (path.is_absolute()) {
        return path;
    }
    return base_dir / path;
}

bool LoadGoldDelays(const std::filesystem::path& path, SpreadingSpecTables* tables, std::string* error_message) {
    CsvData csv;
    if (!ReadCsvFile(path, &csv, error_message)) {
        return false;
    }

    const int prn_index = HeaderIndex(csv.headers, "prn");
    int delay_index = HeaderIndex(csv.headers, "g2_delay");
    if (delay_index < 0) {
        delay_index = HeaderIndex(csv.headers, "g2_delay_chips");
    }

    if (prn_index < 0 || delay_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Gold delay CSV missing required columns: " + path.string();
        }
        return false;
    }

    tables->g2_delays.assign(kMaxPrns, -1);
    for (const auto& row : csv.rows) {
        int prn = 0;
        int delay = 0;
        if (!ParseIntStrict(row[prn_index], &prn) || !ParseIntStrict(row[delay_index], &delay)) {
            continue;
        }
        if (prn >= 1 && prn <= kMaxPrns) {
            tables->g2_delays[prn - 1] = delay;
        }
    }

    for (int prn = 1; prn <= kMaxPrns; ++prn) {
        if (tables->g2_delays[prn - 1] < 0) {
            if (error_message != nullptr) {
                *error_message = "Missing G2 delay for PRN " + std::to_string(prn);
            }
            return false;
        }
    }

    return true;
}

bool LoadWeilPrimary(const std::filesystem::path& path, SpreadingSpecTables* tables, std::string* error_message) {
    CsvData csv;
    if (!ReadCsvFile(path, &csv, error_message)) {
        return false;
    }

    const int prn_index = HeaderIndex(csv.headers, "prn");
    const int k_index = HeaderIndex(csv.headers, "weil_index_k");
    const int p_index = HeaderIndex(csv.headers, "insertion_index_p");
    if (prn_index < 0 || k_index < 0 || p_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Weil primary CSV missing required columns: " + path.string();
        }
        return false;
    }

    tables->weil_primary_k.assign(kMaxPrns, -1);
    tables->weil_primary_insert_index.assign(kMaxPrns, -1);

    for (const auto& row : csv.rows) {
        int prn = 0;
        int k = 0;
        int p = 0;
        if (!ParseIntStrict(row[prn_index], &prn) ||
            !ParseIntStrict(row[k_index], &k) ||
            !ParseIntStrict(row[p_index], &p)) {
            continue;
        }

        if (prn >= 1 && prn <= kMaxPrns) {
            tables->weil_primary_k[prn - 1] = k;
            tables->weil_primary_insert_index[prn - 1] = p;
        }
    }

    for (int prn = 1; prn <= kMaxPrns; ++prn) {
        if (tables->weil_primary_k[prn - 1] < 0 || tables->weil_primary_insert_index[prn - 1] < 0) {
            if (error_message != nullptr) {
                *error_message = "Missing Weil primary parameters for PRN " + std::to_string(prn);
            }
            return false;
        }
    }

    return true;
}

bool LoadWeilTertiary(const std::filesystem::path& path, SpreadingSpecTables* tables, std::string* error_message) {
    CsvData csv;
    if (!ReadCsvFile(path, &csv, error_message)) {
        return false;
    }

    const int prn_index = HeaderIndex(csv.headers, "prn");
    const int k_index = HeaderIndex(csv.headers, "weil_index_k");
    if (prn_index < 0 || k_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Weil tertiary CSV missing required columns: " + path.string();
        }
        return false;
    }

    tables->weil_tertiary_k.assign(kMaxPrns, -1);
    for (const auto& row : csv.rows) {
        int prn = 0;
        int k = 0;
        if (!ParseIntStrict(row[prn_index], &prn) || !ParseIntStrict(row[k_index], &k)) {
            continue;
        }
        if (prn >= 1 && prn <= kMaxPrns) {
            tables->weil_tertiary_k[prn - 1] = k;
        }
    }

    for (int prn = 1; prn <= kMaxPrns; ++prn) {
        if (tables->weil_tertiary_k[prn - 1] < 0) {
            if (error_message != nullptr) {
                *error_message = "Missing Weil tertiary parameter for PRN " + std::to_string(prn);
            }
            return false;
        }
    }

    return true;
}

bool LoadSecondaryCodes(const std::filesystem::path& path, SpreadingSpecTables* tables, std::string* error_message) {
    CsvData csv;
    if (!ReadCsvFile(path, &csv, error_message)) {
        return false;
    }

    const int id_index = HeaderIndex(csv.headers, "id");
    const int chip0_index = HeaderIndex(csv.headers, "chip0");
    const int chip1_index = HeaderIndex(csv.headers, "chip1");
    const int chip2_index = HeaderIndex(csv.headers, "chip2");
    const int chip3_index = HeaderIndex(csv.headers, "chip3");

    if (id_index < 0 || chip0_index < 0 || chip1_index < 0 || chip2_index < 0 || chip3_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Secondary code CSV missing required columns: " + path.string();
        }
        return false;
    }

    std::array<bool, 4> seen{false, false, false, false};
    for (const auto& row : csv.rows) {
        const int secondary_index = SecondaryCodeIndexFromId(row[id_index]);
        if (secondary_index < 0) {
            continue;
        }

        int chip0 = 0;
        int chip1 = 0;
        int chip2 = 0;
        int chip3 = 0;
        if (!ParseIntStrict(row[chip0_index], &chip0) ||
            !ParseIntStrict(row[chip1_index], &chip1) ||
            !ParseIntStrict(row[chip2_index], &chip2) ||
            !ParseIntStrict(row[chip3_index], &chip3)) {
            continue;
        }

        tables->secondary_codes[secondary_index] = {
            static_cast<uint8_t>(chip0 & 1),
            static_cast<uint8_t>(chip1 & 1),
            static_cast<uint8_t>(chip2 & 1),
            static_cast<uint8_t>(chip3 & 1),
        };
        seen[secondary_index] = true;
    }

    for (int i = 0; i < 4; ++i) {
        if (!seen[static_cast<size_t>(i)]) {
            if (error_message != nullptr) {
                *error_message = "Missing secondary code S" + std::to_string(i);
            }
            return false;
        }
    }

    return true;
}

void InitializeSecondaryAssignments(SpreadingSpecTables* tables, int default_index) {
    tables->secondary_code_by_prn.assign(kMaxPrns, default_index);
}

bool LoadTable11Assignments(const std::filesystem::path& path,
                            SpreadingSpecTables* tables,
                            std::string* error_message) {
    if (!std::filesystem::exists(path)) {
        return true;
    }

    CsvData csv;
    if (!ReadCsvFile(path, &csv, error_message)) {
        return false;
    }

    const int prn_q_index = HeaderIndex(csv.headers, "prn_q");
    const int secondary_index = HeaderIndex(csv.headers, "secondary_code");
    if (prn_q_index < 0 || secondary_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Table 11 CSV missing required columns: " + path.string();
        }
        return false;
    }

    for (const auto& row : csv.rows) {
        int prn_q = 0;
        if (!ParseIntStrict(row[prn_q_index], &prn_q)) {
            continue;
        }
        if (prn_q < 1 || prn_q > kMaxPrns) {
            continue;
        }

        const int resolved = SecondaryCodeIndexFromId(row[secondary_index]);
        if (resolved >= 0) {
            tables->secondary_code_by_prn[prn_q - 1] = resolved;
        }
    }

    return true;
}

}  // namespace

bool SpreadingSpecTables::IsReady() const {
    return g2_delays.size() == static_cast<size_t>(kMaxPrns) &&
           weil_primary_k.size() == static_cast<size_t>(kMaxPrns) &&
           weil_primary_insert_index.size() == static_cast<size_t>(kMaxPrns) &&
           weil_tertiary_k.size() == static_cast<size_t>(kMaxPrns) &&
           secondary_code_by_prn.size() == static_cast<size_t>(kMaxPrns);
}

int SpreadingSpecTables::SecondaryIndexForPrn(int prn) const {
    if (prn < 1 || prn > kMaxPrns || secondary_code_by_prn.empty()) {
        return -1;
    }
    return secondary_code_by_prn[prn - 1];
}

int SecondaryCodeIndexFromId(const std::string& id) {
    if (id.size() < 2 || (id[0] != 'S' && id[0] != 's')) {
        return -1;
    }

    if (id[1] < '0' || id[1] > '3') {
        return -1;
    }

    return id[1] - '0';
}

bool LoadSpreadingConfig(const std::string& config_path,
                         SpreadingSpecTables* out_tables,
                         Annex3Paths* out_annex3_paths,
                         std::string* error_message) {
    if (out_tables == nullptr || out_annex3_paths == nullptr) {
        if (error_message != nullptr) {
            *error_message = "LoadSpreadingConfig received null output pointers";
        }
        return false;
    }

    std::unordered_map<std::string, std::string> config_values;
    if (!ParseKeyValueFile(config_path, &config_values, error_message)) {
        return false;
    }

    const std::filesystem::path config_file(config_path);
    const std::filesystem::path config_dir = std::filesystem::absolute(config_file).parent_path();

    SpreadingSpecTables tables;
    Annex3Paths annex3;

    const std::filesystem::path spec_tables_dir = ResolvePath(
        config_dir, ValueOrDefault(config_values, "spec_tables_dir", "docs/spec_tables"));

    if (!LoadGoldDelays(spec_tables_dir / "appendix_c_gold_g2_delays.csv", &tables, error_message)) {
        return false;
    }
    if (!LoadWeilPrimary(spec_tables_dir / "appendix_d_weil_primary_params.csv", &tables, error_message)) {
        return false;
    }
    if (!LoadWeilTertiary(spec_tables_dir / "appendix_e_weil_tertiary_params.csv", &tables, error_message)) {
        return false;
    }
    if (!LoadSecondaryCodes(spec_tables_dir / "table_10_secondary_codes.csv", &tables, error_message)) {
        return false;
    }

    const int default_secondary_index = SecondaryCodeIndexFromId(
        ValueOrDefault(config_values, "afs_q_secondary_code_id", "S0"));
    if (default_secondary_index < 0) {
        if (error_message != nullptr) {
            *error_message = "Invalid afs_q_secondary_code_id value";
        }
        return false;
    }

    InitializeSecondaryAssignments(&tables, default_secondary_index);
    if (!LoadTable11Assignments(spec_tables_dir / "table_11_code_assignments.csv", &tables, error_message)) {
        return false;
    }

    const std::string max_chips_text = ValueOrDefault(config_values, "afs_q_max_chips", "0");
    if (!ParseSizeStrict(max_chips_text, &tables.afs_q_max_chips)) {
        if (error_message != nullptr) {
            *error_message = "Invalid afs_q_max_chips value";
        }
        return false;
    }

    std::filesystem::path annex3_dir = config_dir;
    const std::string annex3_dir_raw = ValueOrDefault(config_values, "annex3_dir", "");
    if (!annex3_dir_raw.empty()) {
        annex3_dir = ResolvePath(config_dir, annex3_dir_raw);
    }

    annex3.gold = ResolvePath(annex3_dir, ValueOrDefault(config_values, "annex3_gold", "006_GoldCode2046hex210prns.txt")).string();
    annex3.weil_primary = ResolvePath(annex3_dir, ValueOrDefault(config_values, "annex3_weil_primary", "007_l1cp_hex210prns.txt")).string();
    annex3.weil_tertiary = ResolvePath(annex3_dir, ValueOrDefault(config_values, "annex3_weil_tertiary", "008_Weil1500hex210prns.txt")).string();

    if (!tables.IsReady()) {
        if (error_message != nullptr) {
            *error_message = "Loaded spec tables are incomplete";
        }
        return false;
    }

    *out_tables = std::move(tables);
    *out_annex3_paths = std::move(annex3);
    return true;
}

}  // namespace lunanet::gateway1
