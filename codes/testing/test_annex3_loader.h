#ifndef LUNANET_TESTING_TEST_ANNEX3_LOADER_H
#define LUNANET_TESTING_TEST_ANNEX3_LOADER_H

#include <filesystem>
#include <string>
#include <vector>

namespace lunanet::testing {

/**
 * Loads an Annex3 reference hex file and extracts one hex string per PRN.
 *
 * Filters out header lines (e.g. "GoldCode2046", "Weil1500"), brackets,
 * and whitespace. Returns false if the parsed count does not match
 * expected_count.
 */
bool LoadAnnex3HexLines(
    const std::filesystem::path& path,
    int expected_count,
    std::vector<std::string>& out,
    std::string& error);

}  // namespace lunanet::testing

#endif  // LUNANET_TESTING_TEST_ANNEX3_LOADER_H
