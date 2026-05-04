#include "test_validators.h"

#include "spreading_codes.h"

#include <algorithm>
#include <chrono>

namespace lunanet::testing {

namespace {

size_t FirstMismatchIndex(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    const size_t limit = std::min(a.size(), b.size());
    for (size_t i = 0; i < limit; ++i) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return limit;
}

}  // namespace

void ValidateAnnex3Suite(
    const std::string& suite_label,
    const std::vector<std::string>& reference_hex,
    size_t expected_chips,
    int max_prns,
    CodeGenerator generator,
    TestReporter& reporter) {

    for (int prn = 1; prn <= max_prns; ++prn) {
        using Clock = std::chrono::high_resolution_clock;
        const auto start = Clock::now();

        const std::vector<uint8_t> expected =
            lunanet::hex_to_vector(reference_hex[prn - 1], expected_chips);
        const std::vector<uint8_t> actual = generator(prn);

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(Clock::now() - start).count();

        const std::string test_name = "PRN " + std::to_string(prn);

        if (expected.size() != expected_chips) {
            reporter.Record({suite_label, test_name, TestStatus::kFail,
                "Reference decode error: got " + std::to_string(expected.size()) + " chips",
                elapsed_ms});
            continue;
        }

        if (actual.size() != expected_chips) {
            reporter.Record({suite_label, test_name, TestStatus::kFail,
                "Length mismatch: got " + std::to_string(actual.size()) +
                ", expected " + std::to_string(expected_chips),
                elapsed_ms});
            continue;
        }

        if (actual != expected) {
            const size_t mismatch = FirstMismatchIndex(actual, expected);
            reporter.Record({suite_label, test_name, TestStatus::kFail,
                "Mismatch at chip " + std::to_string(mismatch),
                elapsed_ms});
            continue;
        }

        reporter.Record({suite_label, test_name, TestStatus::kPass, "", elapsed_ms});
    }
}

void ValidateCodeLength(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& code,
    size_t expected_length,
    TestReporter& reporter) {

    if (code.size() == expected_length) {
        reporter.Record({suite, name, TestStatus::kPass, "", 0.0});
    } else {
        reporter.Record({suite, name, TestStatus::kFail,
            "Length " + std::to_string(code.size()) +
            ", expected " + std::to_string(expected_length), 0.0});
    }
}

void ValidateBoundsRejection(
    const std::string& suite,
    const std::string& name,
    CodeGenerator generator,
    int invalid_prn,
    TestReporter& reporter) {

    const std::vector<uint8_t> result = generator(invalid_prn);
    if (result.empty()) {
        reporter.Record({suite, name, TestStatus::kPass, "", 0.0});
    } else {
        reporter.Record({suite, name, TestStatus::kFail,
            "Expected empty for PRN " + std::to_string(invalid_prn) +
            ", got " + std::to_string(result.size()) + " chips", 0.0});
    }
}

void ValidateEquality(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& actual,
    const std::vector<uint8_t>& expected,
    TestReporter& reporter) {

    if (actual == expected) {
        reporter.Record({suite, name, TestStatus::kPass, "", 0.0});
    } else {
        const size_t mismatch = FirstMismatchIndex(actual, expected);
        reporter.Record({suite, name, TestStatus::kFail,
            "Mismatch at index " + std::to_string(mismatch) +
            " (actual_len=" + std::to_string(actual.size()) +
            ", expected_len=" + std::to_string(expected.size()) + ")", 0.0});
    }
}

void ValidateNonEmpty(
    const std::string& suite,
    const std::string& name,
    const std::vector<uint8_t>& code,
    TestReporter& reporter) {

    if (!code.empty()) {
        reporter.Record({suite, name, TestStatus::kPass, "", 0.0});
    } else {
        reporter.Record({suite, name, TestStatus::kFail, "Sequence is empty", 0.0});
    }
}

void ValidateCondition(
    const std::string& suite,
    const std::string& name,
    bool condition,
    const std::string& failure_detail,
    TestReporter& reporter) {

    if (condition) {
        reporter.Record({suite, name, TestStatus::kPass, "", 0.0});
    } else {
        reporter.Record({suite, name, TestStatus::kFail, failure_detail, 0.0});
    }
}

void ValidateConditionTimed(
    const std::string& suite,
    const std::string& name,
    bool condition,
    const std::string& failure_detail,
    double elapsed_ms,
    TestReporter& reporter) {

    if (condition) {
        reporter.Record({suite, name, TestStatus::kPass, "", elapsed_ms});
    } else {
        reporter.Record({suite, name, TestStatus::kFail, failure_detail, elapsed_ms});
    }
}

void ValidateTable11Suite(
    const std::vector<Table11Entry>& entries,
    const std::vector<int>& secondary_code_by_prn,
    const std::array<std::array<uint8_t, 4>, 4>& secondary_codes,
    CodeGenerator gen_gold,
    CodeGenerator gen_weil_primary,
    CodeGenerator gen_weil_tertiary,
    CodeGenerator gen_afs_q,
    TestReporter& reporter) {

    const std::string kSuite = "Table11/Assignments";

    for (const auto& entry : entries) {
        const std::string node_label =
            "Node " + std::to_string(entry.node_id);

        // 1. Verify secondary code assignment matches Table 11
        const int expected_sec = entry.secondary_code_index;
        const int actual_sec = (entry.prn_q >= 1 &&
            entry.prn_q <= static_cast<int>(secondary_code_by_prn.size()))
            ? secondary_code_by_prn[entry.prn_q - 1]
            : -1;

        ValidateCondition(
            kSuite, node_label + " secondary=S" + std::to_string(expected_sec),
            actual_sec == expected_sec,
            "Config has S" + std::to_string(actual_sec) +
            ", Table 11 expects S" + std::to_string(expected_sec),
            reporter);

        // 2. AFS-I generates non-empty for assigned prn_i
        const auto gold = gen_gold(entry.prn_i);
        ValidateCondition(
            kSuite, node_label + " AFS-I PRN " + std::to_string(entry.prn_i),
            !gold.empty(),
            "Gold code generation failed for PRN " + std::to_string(entry.prn_i),
            reporter);

        // 3. AFS-Q primary generates non-empty for assigned prn_q
        const auto weil_p = gen_weil_primary(entry.prn_q);
        ValidateCondition(
            kSuite, node_label + " AFS-Q primary PRN " + std::to_string(entry.prn_q),
            !weil_p.empty(),
            "Weil primary generation failed for PRN " + std::to_string(entry.prn_q),
            reporter);

        // 4. AFS-Q tertiary generates non-empty for assigned tertiary_prn
        const auto weil_t = gen_weil_tertiary(entry.tertiary_prn);
        ValidateCondition(
            kSuite, node_label + " AFS-Q tertiary PRN " + std::to_string(entry.tertiary_prn),
            !weil_t.empty(),
            "Weil tertiary generation failed for PRN " + std::to_string(entry.tertiary_prn),
            reporter);

        // 5. Tiered AFS-Q structural verification:
        //    Verify first primary-code epoch matches primary ⊕ secondary[0] ⊕ tertiary[0]
        if (!weil_p.empty() && !weil_t.empty() && actual_sec >= 0 && actual_sec <= 3) {
            using Clock = std::chrono::high_resolution_clock;
            const auto start = Clock::now();

            const auto afs_q = gen_afs_q(entry.prn_q);
            const double elapsed_ms =
                std::chrono::duration<double, std::milli>(Clock::now() - start).count();

            if (afs_q.empty()) {
                reporter.Record({kSuite,
                    node_label + " AFS-Q tiered construction",
                    TestStatus::kFail, "AFS-Q generation returned empty", elapsed_ms});
                continue;
            }

            // Check first primary-code-length chips against manual ⊕
            const size_t check_len = std::min(weil_p.size(), afs_q.size());
            const uint8_t sec_chip = secondary_codes[actual_sec][0];
            const uint8_t ter_chip = weil_t[0];
            bool structural_ok = true;
            size_t mismatch_at = 0;

            for (size_t i = 0; i < check_len; ++i) {
                const uint8_t expected_chip =
                    static_cast<uint8_t>(weil_p[i] ^ sec_chip ^ ter_chip);
                if (afs_q[i] != expected_chip) {
                    structural_ok = false;
                    mismatch_at = i;
                    break;
                }
            }

            ValidateConditionTimed(
                kSuite, node_label + " AFS-Q tiered construction",
                structural_ok,
                "Tiered mismatch at chip " + std::to_string(mismatch_at),
                elapsed_ms, reporter);
        }
    }
}

}  // namespace lunanet::testing
