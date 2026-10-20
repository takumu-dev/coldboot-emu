// SPDX-License-Identifier: GPL-3.0-or-later

// The main program of a precondition test. A precondition that fires calls assertion_failed, which
// here reports the failure and exits with a failing status. CTest expects the test to fail; if the
// precondition doesn't fire, the program succeeds and the test fails.
//
// The handler doesn't abort, since CTest counts an abort as a crash even in a test expected to
// fail.

#include "assertion_report.hpp"
#include "precondition_test.hpp"
#include <coldboot/core/assertion.hpp>

#include <cstdlib>

void coldboot::assertion_failed(const AssertionFailure& failure) {
    test::report_assertion_failure(failure);
    std::_Exit(EXIT_FAILURE);
}

int main() {
    break_precondition();
    return EXIT_SUCCESS;
}
