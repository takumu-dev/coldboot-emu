// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// Checks of preconditions and invariants, and the function a failed check calls.
///
/// A broken precondition is a bug in the caller, not an error to return. Nothing the guest does is
/// ever one: the core masks every guest value as the hardware does, and meets what it can't do with
/// a stop. `COLDBOOT_ASSERT` checks the core's interface and the frontends in every build, release
/// included. `COLDBOOT_DEBUG_ASSERT` checks inside the core, outside release builds, since the
/// core's components sit on the per-instruction path.
///
/// A failed check calls `coldboot::assertion_failed`, which the core declares and never defines.
/// Every program that links the core defines it once, to report the failure where that program
/// can, and it never returns. It is the one way out of the core during a run besides the
/// diagnostics sink and the run's result.

#pragma once

#include <coldboot/core/literal.hpp>

#include <source_location>
#include <span>

namespace coldboot {

/// A failed check, as `assertion_failed` receives it.
struct AssertionFailure {
    /// The condition that was false, as it is written in the check.
    std::span<const char> condition;

    /// What must hold.
    std::span<const char> message;

    /// Where the check is written, with the path relative to the repository.
    std::source_location location;
};

/// Reports a failed check and ends the program, without returning.
///
/// Every program that links the core defines it once.
///
/// \param failure The check that failed.
[[noreturn]] void assertion_failed(const AssertionFailure& failure);

} // namespace coldboot

// NOLINTBEGIN(cppcoreguidelines-macro-usage): a check reports its condition as written and the
// place it is written, which only a macro can see.

/// Checks a precondition of the core's interface or a frontend's code, in every build.
///
/// The condition is evaluated once. If it is false, `coldboot::assertion_failed` is called, which
/// doesn't return.
///
/// \param condition The condition that must hold.
/// \param message What must hold, as a string literal.
#define COLDBOOT_ASSERT(condition, message)                                                        \
    (static_cast<bool>(condition)                                                                  \
         ? static_cast<void>(0)                                                                    \
         : ::coldboot::assertion_failed(::coldboot::AssertionFailure{                              \
               ::coldboot::literal(#condition), ::coldboot::literal(message),                      \
               ::std::source_location::current()}))

/// Checks an invariant inside the core, outside release builds.
///
/// Where `NDEBUG` is defined, as in release builds, the condition is never evaluated, though it
/// must still compile. Elsewhere this is `COLDBOOT_ASSERT`.
///
/// \param condition The condition that must hold.
/// \param message What must hold, as a string literal.
#ifdef NDEBUG
#define COLDBOOT_DEBUG_ASSERT(condition, message)                                                  \
    static_cast<void>(sizeof(static_cast<bool>(condition)))
#else
#define COLDBOOT_DEBUG_ASSERT(condition, message) COLDBOOT_ASSERT(condition, message)
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
