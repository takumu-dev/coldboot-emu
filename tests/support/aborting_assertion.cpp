// SPDX-License-Identifier: GPL-3.0-or-later

// The failed-check handler of the Catch2 test programs and the test tools: it reports the failure
// and aborts, as the desktop frontend does. CTest runs each test case as a process of its own, so
// an abort fails that case alone.

#include "assertion_report.hpp"
#include <coldboot/core/assertion.hpp>

#include <cstdlib>

void coldboot::assertion_failed(const AssertionFailure& failure) {
    test::report_assertion_failure(failure);
    std::abort();
}
