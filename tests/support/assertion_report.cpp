// SPDX-License-Identifier: GPL-3.0-or-later

#include "assertion_report.hpp"

#include <coldboot/core/assertion.hpp>

#include <cstdio> // NOLINT(misc-include-cleaner): stderr
#include <print>
#include <string_view>

namespace coldboot::test {

void report_assertion_failure(const AssertionFailure& failure) {
    std::println(stderr, "{}:{}: assertion failed: {} ({})", failure.location.file_name(),
                 failure.location.line(),
                 std::string_view{failure.message.begin(), failure.message.end()},
                 std::string_view{failure.condition.begin(), failure.condition.end()});
}

} // namespace coldboot::test
