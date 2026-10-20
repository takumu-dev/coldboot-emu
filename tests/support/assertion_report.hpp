// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// How test programs report a failed check.

#pragma once

#include <coldboot/core/assertion.hpp>

/// What the tests share.
namespace coldboot::test {

/// Writes a failed check to stderr as one line: where it is written, what must hold, and the
/// condition as written.
///
/// \param failure The check that failed.
void report_assertion_failure(const AssertionFailure& failure);

} // namespace coldboot::test
