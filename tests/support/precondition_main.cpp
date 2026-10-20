// SPDX-License-Identifier: GPL-3.0-or-later

// The main program of a precondition test. A precondition that fires calls assertion_failed, which
// here reports the failure and exits with a failing status. CTest passes the test only on the
// report of the precondition under test, so it fails if that precondition doesn't fire, or if
// another check fires first.
//
// The handler doesn't abort, since CTest counts an abort as a crash whatever the test expects.

#include "assertion_report.hpp"
#include "precondition_test.hpp"
#include <coldboot/core/assertion.hpp>

#include <cstdlib>

void coldboot::assertion_failed(const AssertionFailure& failure) {
    test::report_assertion_failure(failure);
    std::_Exit(EXIT_FAILURE);
}

int main() {
    coldboot::test::break_precondition();
    return EXIT_SUCCESS;
}
