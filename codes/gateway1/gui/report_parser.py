"""JUnit XML and Markdown report parser for Gateway 1 test results."""

from __future__ import annotations

import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class TestCase:
    """A single test case result."""

    suite: str
    name: str
    status: str
    time_s: float
    detail: str = ""


@dataclass
class SuiteStats:
    """Aggregate statistics for a test suite."""

    name: str
    passed: int = 0
    failed: int = 0
    skipped: int = 0
    time_s: float = 0.0

    @property
    def total(self) -> int:
        return self.passed + self.failed + self.skipped


@dataclass
class ReportData:
    """Parsed report containing summary, suite stats, and individual test cases."""

    suites: list[SuiteStats] = field(default_factory=list)
    testcases: list[TestCase] = field(default_factory=list)
    markdown: str = ""
    total_tests: int = 0
    total_passed: int = 0
    total_failed: int = 0
    total_skipped: int = 0
    total_time_s: float = 0.0

    @property
    def all_passed(self) -> bool:
        return self.total_failed == 0


def parse_junit_xml(path: Path) -> ReportData:
    """Parse a JUnit XML file into structured report data."""
    tree = ET.parse(path)
    root = tree.getroot()

    report = ReportData()

    for ts in root.iter("testsuite"):
        tests = int(ts.get("tests", 0))
        failures = int(ts.get("failures", 0))
        skipped = int(ts.get("skipped", 0))
        time_s = float(ts.get("time", 0))

        suite = SuiteStats(
            name=ts.get("name", "Unknown"),
            passed=tests - failures - skipped,
            failed=failures,
            skipped=skipped,
            time_s=time_s,
        )
        report.suites.append(suite)

        for tc in ts.findall("testcase"):
            failure_el = tc.find("failure")
            skip_el = tc.find("skipped")

            if failure_el is not None:
                status = "FAIL"
                detail = failure_el.get("message", failure_el.text or "")
            elif skip_el is not None:
                status = "SKIP"
                detail = ""
            else:
                status = "PASS"
                detail = ""

            report.testcases.append(
                TestCase(
                    suite=tc.get("classname", ""),
                    name=tc.get("name", ""),
                    time_s=float(tc.get("time", 0)),
                    status=status,
                    detail=detail,
                )
            )

    report.total_tests = sum(s.total for s in report.suites)
    report.total_passed = sum(s.passed for s in report.suites)
    report.total_failed = sum(s.failed for s in report.suites)
    report.total_skipped = sum(s.skipped for s in report.suites)
    report.total_time_s = sum(s.time_s for s in report.suites)

    return report


def load_markdown(path: Path) -> str:
    """Read markdown report content, returning empty string if unavailable."""
    if path.exists():
        return path.read_text(encoding="utf-8")
    return ""
