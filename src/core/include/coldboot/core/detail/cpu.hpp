// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The CPU: an interpreter of the R3000A's instructions.

#pragma once

#include <coldboot/core/cpu_state.hpp>
#include <coldboot/core/detail/bus.hpp>
#include <coldboot/core/stop_report.hpp>

#include <array>
#include <cstdint>
#include <optional>

namespace coldboot::detail {

/// Where the CPU starts at power-on, in the BIOS ROM through KSEG1.
inline constexpr std::uint32_t reset_vector = 0xBFC0'0000;

/// The R3000A, running one instruction at a time against the bus.
///
/// It decodes SLL, LUI, ORI, ADDIU and SW. Any other instruction is absent.
class Cpu {
  public:
    /// Runs the instruction at the PC.
    ///
    /// \param bus The bus it fetches from and stores through.
    /// \param now The emulated time at which the instruction begins.
    /// \return The stop report, when the instruction meets something absent. It isn't then carried
    ///         out, and the CPU is as it was.
    [[nodiscard]] std::optional<StopReport> step(Bus& bus, std::uint64_t now);

    /// Reads the registers.
    ///
    /// \return A copy of them.
    [[nodiscard]] CpuState state() const;

  private:
    /// The general-purpose registers, r0 to r31. r0 is never written, so it stays 0.
    std::array<std::uint32_t, 32> gpr_{};

    /// The address of the next instruction.
    std::uint32_t pc_ = reset_vector;
};

} // namespace coldboot::detail
