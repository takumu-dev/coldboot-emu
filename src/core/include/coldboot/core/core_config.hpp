// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// What a frontend gives the core when it creates it.

#pragma once

#include <coldboot/core/diagnostics.hpp>

#include <cstddef>
#include <cstdint>
#include <span>

namespace coldboot {

/// The size of a BIOS, the PS1's 512 KiB boot ROM.
inline constexpr std::size_t bios_size = 524'288;

/// The emulated console's GPU clock, which sets its video timing.
enum class VideoClock : std::uint8_t {
    ntsc, ///< An NTSC console's.
    pal,  ///< A PAL console's.
};

/// What a frontend gives the core when it creates it. Creating a core can't fail.
struct CoreConfig {
    /// The BIOS the core runs. The core copies it, so the frontend may free its buffer at once. A
    /// BIOS that comes out short is padded to its size where it is built.
    std::span<const std::byte, bios_size> bios;

    /// The emulated console's video clock, fixed for the core's life.
    VideoClock video_clock{};

    /// The sink of the core's diagnostics, which must outlive the core.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members): a sink must be given
    Diagnostics& diagnostics;
};

} // namespace coldboot
