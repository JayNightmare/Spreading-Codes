#ifndef LUNANET_TESTING_TEST_VALIDATORS_H
#define LUNANET_TESTING_TEST_VALIDATORS_H

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "test_reporter.h"

namespace lunanet::testing {

using CodeGenerator = std::function<std::vector<uint8_t>(int)>;

/**
 * Validates a generator against Annex3 hex reference vectors for all PRNs.
 * Records one TestResult per PRN into the reporter.
 */
void ValidateAnnex3Suite(
    const std::string& suite_label,
    const std::vector<std::string>& reference_hex,
    size_t expected_chips,
    int max_prns,
    CodeGenerator generator,
    TestReporter& reporter);

void ValidateCodeLength(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& code,
    size_t expected_length,
    TestReporter& reporter);

void ValidateBoundsRejection(
    const std::string& suite,
    const std::string& name,
    CodeGenerator generator,
    int invalid_prn,
    TestReporter& reporter);

void ValidateEquality(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& actual,
    const std::vector<uint8_t>& expected,
    TestReporter& reporter);

void ValidateNonEmpty(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& code,
    TestReporter& reporter);

void ValidateCondition(
    const std::string& suite,
    const std::string& name,
    bool condition,
    const std::string& failure_detail,
    TestReporter& reporter);

void ValidateConditionTimed(
    const std::string& suite,
    const std::string& name,
    bool condition,
    const std::string& failure_detail,
    double elapsed_ms,
    TestReporter& reporter);

struct Table11Entry {
    int node_id;
    int prn_i;
    int prn_q;
    int secondary_code_index;  // 0-3 for S0-S3
    int tertiary_prn;
    int phase_offset;
};

/**
 * Validates Table 11 interim code assignments (SC-1.6) for 12 LNSP nodes.
 * Checks secondary code cycling, AFS-I/Q PRN identity, and tiered construction.
 */
void ValidateTable11Suite(
    const std::vector<Table11Entry>& entries,
    const std::vector<int>& secondary_code_by_prn,
    const std::array<std::array<uint8_t, 4>, 4>& secondary_codes,
    CodeGenerator gen_gold,
    CodeGenerator gen_weil_primary,
    CodeGenerator gen_weil_tertiary,
    CodeGenerator gen_afs_q,
    TestReporter& reporter);

}  // namespace lunanet::testing

#endif  // LUNANET_TESTING_TEST_VALIDATORS_H
