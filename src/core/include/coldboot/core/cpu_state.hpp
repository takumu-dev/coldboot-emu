// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The CPU's registers, as the core gives them between runs.

#pragma once

#include <array>
#include <cstdint>

namespace coldboot {

/// The CPU's registers, read between runs.
struct CpuState {
    /// The general-purpose registers r0 to r31, indexed by number. r0 always reads 0.
    std::array<std::uint32_t, 32> gpr{};

    /// The address of the next instruction to run. After a stop, it is the instruction that
    /// stopped, which wasn't carried out.
    std::uint32_t pc{};
};

} // namespace coldboot
