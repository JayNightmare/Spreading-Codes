#include "test_reporter.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace lunanet::testing {

void TestReporter::Record(TestResult result) {
    results_.push_back(std::move(result));
}

bool TestReporter::AllPassed() const {
    return FailCount() == 0;
}

size_t TestReporter::PassCount() const {
    return static_cast<size_t>(std::count_if(
        results_.begin(), results_.end(),
        [](const TestResult& r) { return r.status == TestStatus::kPass; }));
}

size_t TestReporter::FailCount() const {
    return static_cast<size_t>(std::count_if(
        results_.begin(), results_.end(),
        [](const TestResult& r) { return r.status == TestStatus::kFail; }));
}

size_t TestReporter::SkipCount() const {
    return static_cast<size_t>(std::count_if(
        results_.begin(), results_.end(),
        [](const TestResult& r) { return r.status == TestStatus::kSkip; }));
}

size_t TestReporter::TotalCount() const {
    return results_.size();
}

const std::vector<TestResult>& TestReporter::Results() const {
    return results_;
}

std::vector<std::string> TestReporter::OrderedSuiteNames() const {
    std::vector<std::string> names;
    std::unordered_set<std::string> seen;
    for (const auto& r : results_) {
        if (seen.insert(r.suite).second) {
            names.push_back(r.suite);
        }
    }
    return names;
}

TestReporter::SuiteStats TestReporter::StatsForSuite(const std::string& suite) const {
    SuiteStats stats;
    for (const auto& r : results_) {
        if (r.suite != suite) {
            continue;
        }
        switch (r.status) {
            case TestStatus::kPass: ++stats.pass; break;
            case TestStatus::kFail: ++stats.fail; break;
            case TestStatus::kSkip: ++stats.skip; break;
        }
        stats.total_ms += r.elapsed_ms;
    }
    return stats;
}

void TestReporter::PrintSummary(std::ostream& out) const {
    const auto suites = OrderedSuiteNames();

    for (const auto& suite : suites) {
        const SuiteStats stats = StatsForSuite(suite);
        const size_t total = stats.pass + stats.fail + stats.skip;

        if (stats.fail == 0) {
            out << suite << " reference validation: PASS ("
                << stats.pass << "/" << total << ")" << std::endl;
        } else {
            out << suite << " reference validation: FAIL ("
                << stats.fail << "/" << total << " mismatched)" << std::endl;

            size_t printed = 0;
            for (const auto& r : results_) {
                if (r.suite == suite && r.status == TestStatus::kFail && printed < 5) {
                    out << "  " << r.name << ": " << r.detail << std::endl;
                    ++printed;
                }
            }
        }
    }

    out << std::endl;
    if (AllPassed()) {
        out << "Gateway 1 modular foundation checks passed. ("
            << PassCount() << "/" << TotalCount() << " tests)" << std::endl;
    } else {
        out << "FAILURES DETECTED: " << FailCount() << " of "
            << TotalCount() << " tests failed." << std::endl;
    }
}

namespace {

std::string XmlEscape(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (const char ch : input) {
        switch (ch) {
            case '&':  output += "&amp;";  break;
            case '<':  output += "&lt;";   break;
            case '>':  output += "&gt;";   break;
            case '"':  output += "&quot;"; break;
            case '\'': output += "&apos;"; break;
            default:   output.push_back(ch); break;
        }
    }
    return output;
}

}  // namespace

bool TestReporter::WriteMarkdownReport(const std::filesystem::path& path) const {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << "# Gateway 1 — Test Results\n\n";
    out << "**Overall**: " << (AllPassed() ? "✅ PASS" : "❌ FAIL") << "\n\n";

    out << "## Summary\n\n";
    out << "| Suite | Passed | Failed | Skipped | Total | Time (ms) |\n";
    out << "|-------|--------|--------|---------|-------|-----------|\n";

    const auto suites = OrderedSuiteNames();
    double grand_total_ms = 0.0;

    for (const auto& suite : suites) {
        const SuiteStats stats = StatsForSuite(suite);
        const size_t total = stats.pass + stats.fail + stats.skip;
        grand_total_ms += stats.total_ms;

        out << "| " << suite
            << " | " << stats.pass
            << " | " << stats.fail
            << " | " << stats.skip
            << " | " << total
            << " | " << std::fixed << std::setprecision(1) << stats.total_ms
            << " |\n";
    }

    out << "| **Total** | **" << PassCount()
        << "** | **" << FailCount()
        << "** | **" << SkipCount()
        << "** | **" << TotalCount()
        << "** | **" << std::fixed << std::setprecision(1) << grand_total_ms
        << "** |\n\n";

    const size_t fail_count = FailCount();
    out << "## Failures\n\n";
    if (fail_count == 0) {
        out << "(none)\n";
    } else {
        out << "| Suite | Test | Detail |\n";
        out << "|-------|------|--------|\n";
        for (const auto& r : results_) {
            if (r.status == TestStatus::kFail) {
                out << "| " << r.suite << " | " << r.name << " | " << r.detail << " |\n";
            }
        }
    }

    return out.good();
}

bool TestReporter::WriteJUnitXml(const std::filesystem::path& path) const {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path);
    if (!out) {
        return false;
    }

    const auto suites = OrderedSuiteNames();
    double grand_total_s = 0.0;
    for (const auto& r : results_) {
        grand_total_s += r.elapsed_ms / 1000.0;
    }

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<testsuites name=\"Gateway1\" tests=\"" << TotalCount()
        << "\" failures=\"" << FailCount()
        << "\" skipped=\"" << SkipCount()
        << "\" time=\"" << std::fixed << std::setprecision(3) << grand_total_s
        << "\">\n";

    for (const auto& suite : suites) {
        const SuiteStats stats = StatsForSuite(suite);
        const size_t total = stats.pass + stats.fail + stats.skip;

        out << "  <testsuite name=\"" << XmlEscape(suite)
            << "\" tests=\"" << total
            << "\" failures=\"" << stats.fail
            << "\" skipped=\"" << stats.skip
            << "\" time=\"" << std::fixed << std::setprecision(3) << (stats.total_ms / 1000.0)
            << "\">\n";

        for (const auto& r : results_) {
            if (r.suite != suite) {
                continue;
            }

            out << "    <testcase name=\"" << XmlEscape(r.name)
                << "\" classname=\"" << XmlEscape(r.suite)
                << "\" time=\"" << std::fixed << std::setprecision(4) << (r.elapsed_ms / 1000.0)
                << "\"";

            if (r.status == TestStatus::kPass) {
                out << "/>\n";
            } else if (r.status == TestStatus::kSkip) {
                out << ">\n      <skipped/>\n    </testcase>\n";
            } else {
                out << ">\n      <failure message=\"" << XmlEscape(r.detail)
                    << "\">" << XmlEscape(r.detail) << "</failure>\n    </testcase>\n";
            }
        }

        out << "  </testsuite>\n";
    }

    out << "</testsuites>\n";
    return out.good();
}

}  // namespace lunanet::testing
