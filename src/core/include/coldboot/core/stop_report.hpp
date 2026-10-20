// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// What the core says about its stop, and the one line of text every frontend prints for it.

#pragma once

#include <coldboot/core/access.hpp>
#include <coldboot/core/text_writer.hpp>

#include <cstddef>
#include <cstdint>

namespace coldboot {

/// The absent part of the PS1 that a stop met.
enum class AbsentPart : std::uint8_t {
    /// An address that maps to nothing.
    unmapped,

    /// An instruction the core doesn't decode. The access is that instruction's fetch.
    instruction,

    /// A load or store at an address that isn't a multiple of its width, which raises an address
    /// error exception that the core doesn't build.
    address_error,
};

/// What the core says about its stop: what was touched, at which address, by which instruction,
/// and when.
///
/// The instruction that met the absent part isn't carried out, so the core's state, emulated time
/// included, is as it was when that instruction began.
struct StopReport {
    /// What was touched.
    AbsentPart part{};

    /// The address touched, in the physical address space, so that a part reached through any
    /// segment is reported at the same address. KUSEG's first 512 MiB, KSEG0 and KSEG1 mirror that
    /// space; an address anywhere else is given as it is.
    std::uint32_t address{};

    /// Whether the access read or wrote.
    AccessDirection direction{};

    /// How many bits the access moved.
    AccessWidth width{};

    /// The address of the instruction that made the access.
    std::uint32_t pc{};

    /// The emulated time at which that instruction began, in CPU cycles since power-on.
    std::uint64_t time{};

    /// Compares every field.
    friend bool operator==(const StopReport&, const StopReport&) = default;
};

/// The most characters any of the core library's lines holds.
inline constexpr std::size_t line_capacity = 128;

/// One of the core library's lines of text, with no line break at its end.
using Line = TextWriter<line_capacity>;

/// The stop report's one-line text form, which every frontend prints alike.
///
/// It reads, for example, `stop: nothing at 1F802020h, 32-bit write, PC BFC0000Ch, emulated time
/// 3`.
///
/// \param report The stop report.
/// \return The line.
[[nodiscard]] Line stop_line(const StopReport& report);

} // namespace coldboot
