#ifndef LUNANET_TESTING_TEST_REPORTER_H
#define LUNANET_TESTING_TEST_REPORTER_H

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace lunanet::testing {

enum class TestStatus { kPass, kFail, kSkip };

struct TestResult {
    std::string suite;
    std::string name;
    TestStatus status;
    std::string detail;
    double elapsed_ms = 0.0;
};

/**
 * Collects test results and writes reports in multiple formats.
 *
 * Supports console summary, markdown file, and JUnit XML output.
 */
class TestReporter {
public:
    void Record(TestResult result);

    void PrintSummary(std::ostream& out) const;
    bool WriteMarkdownReport(const std::filesystem::path& path) const;
    bool WriteJUnitXml(const std::filesystem::path& path) const;

    bool AllPassed() const;
    size_t PassCount() const;
    size_t FailCount() const;
    size_t SkipCount() const;
    size_t TotalCount() const;
    const std::vector<TestResult>& Results() const;

private:
    struct SuiteStats {
        size_t pass = 0;
        size_t fail = 0;
        size_t skip = 0;
        double total_ms = 0.0;
    };

    std::vector<std::string> OrderedSuiteNames() const;
    SuiteStats StatsForSuite(const std::string& suite) const;

    std::vector<TestResult> results_;
};

}  // namespace lunanet::testing

#endif  // LUNANET_TESTING_TEST_REPORTER_H
