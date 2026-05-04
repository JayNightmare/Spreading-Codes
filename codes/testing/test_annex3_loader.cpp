#include "test_annex3_loader.h"

#include <cctype>
#include <fstream>

namespace lunanet::testing {

namespace {

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

std::string ExtractHexToken(const std::string& line) {
    const std::string trimmed = Trim(line);
    if (trimmed.empty()) {
        return {};
    }

    if (trimmed.find("GoldCode2046") != std::string::npos ||
        trimmed.find("Weil1500") != std::string::npos ||
        trimmed == "[" || trimmed == "]" || trimmed == "];" || trimmed == "\\") {
        return {};
    }

    std::string hex;
    hex.reserve(trimmed.size());
    for (const char ch : trimmed) {
        if (std::isxdigit(static_cast<unsigned char>(ch)) != 0) {
            hex.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
    }

    return hex;
}

}  // namespace

bool LoadAnnex3HexLines(
    const std::filesystem::path& path,
    int expected_count,
    std::vector<std::string>& out,
    std::string& error) {

    std::ifstream input(path);
    if (!input) {
        error = "Failed to open reference file: " + path.string();
        return false;
    }

    out.clear();
    std::string line;
    while (std::getline(input, line)) {
        const std::string token = ExtractHexToken(line);
        if (!token.empty()) {
            out.push_back(token);
        }
    }

    if (static_cast<int>(out.size()) != expected_count) {
        error = "Expected " + std::to_string(expected_count) +
                " reference rows in " + path.string() +
                ", got " + std::to_string(out.size());
        return false;
    }

    return true;
}

}  // namespace lunanet::testing
