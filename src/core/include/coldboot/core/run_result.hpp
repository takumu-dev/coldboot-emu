// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// Why a run of the core returned.

#pragma once

#include <coldboot/core/stop_report.hpp>

#include <cstdint>
#include <optional>

namespace coldboot {

/// Why a run returned.
enum class RunEnd : std::uint8_t {
    /// Emulated time reached the run's `until`.
    reached_until,

    /// The core has stopped, in this run or an earlier one.
    stopped,
};

/// Why a run returned, with the stop report for a stop.
struct RunResult {
    /// Why it returned.
    RunEnd end{};

    /// The stop report, present exactly when `end` is `RunEnd::stopped`.
    std::optional<StopReport> stop{};

    /// Compares every field.
    friend bool operator==(const RunResult&, const RunResult&) = default;
};

} // namespace coldboot
