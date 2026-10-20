// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The core: the emulated PS1, which a frontend creates, runs and reads between runs.

#pragma once

#include <coldboot/core/core_config.hpp>
#include <coldboot/core/cpu_state.hpp>
#include <coldboot/core/detail/bus.hpp>
#include <coldboot/core/detail/cpu.hpp>
#include <coldboot/core/run_result.hpp>
#include <coldboot/core/stop_report.hpp>

#include <cstdint>
#include <optional>

namespace coldboot {

/// The emulated PS1: one fixed-size object that holds all of its memory and never allocates.
///
/// A frontend decides where it lives, and never on the stack, which is too small for it: the tests
/// use `std::make_unique`. It can't be copied or moved. It is used from one thread at a time.
///
/// Emulated time counts CPU cycles since power-on, and for now each instruction takes one.
class Core {
  public:
    /// Creates the core at power-on, with the CPU at the reset vector. It can't fail.
    ///
    /// \param config The BIOS, which the core copies, the video clock and the diagnostics sink.
    explicit Core(const CoreConfig& config);

    Core(const Core&) = delete;            ///< The core is never copied.
    Core(Core&&) = delete;                 ///< The core is never moved.
    Core& operator=(const Core&) = delete; ///< The core is never copied.
    Core& operator=(Core&&) = delete;      ///< The core is never moved.

    /// Destroys the core.
    ~Core() = default;

    /// Runs the core until emulated time reaches `until`, or until it stops.
    ///
    /// Every instruction takes at least one cycle, so `run(now() + 1)` runs exactly one. A stop is
    /// final: once the core has stopped, every run returns the same stop report at once.
    ///
    /// \param until The emulated time to run to, which must be later than `now()`.
    /// \return Why the run returned, with the stop report for a stop.
    [[nodiscard]] RunResult run(std::uint64_t until);

    /// Reads emulated time.
    ///
    /// \return The CPU cycles since power-on.
    [[nodiscard]] std::uint64_t now() const;

    /// Reads the CPU's registers, between runs and after a stop.
    ///
    /// \return A copy of them.
    [[nodiscard]] CpuState cpu_state() const;

  private:
    /// The memory map, holding the BIOS ROM and RAM.
    detail::Bus bus_;

    /// The CPU.
    detail::Cpu cpu_;

    /// Emulated time, in CPU cycles since power-on.
    std::uint64_t now_ = 0;

    /// The stop report, once the core has stopped.
    std::optional<StopReport> stop_;
};

} // namespace coldboot
