// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The sink that receives what the core reports while it runs, for a person to read.

#pragma once

#include <coldboot/core/access.hpp>

#include <cstdint>
#include <span>

namespace coldboot {

/// The first read or the first write of a device register.
struct DeviceAccess {
    /// The emulated time of the access, in CPU cycles since power-on.
    std::uint64_t time{};

    /// The address of the instruction that made it.
    std::uint32_t pc{};

    /// Whether it read or wrote.
    AccessDirection direction{};

    /// How many bits it moved.
    AccessWidth width{};

    /// The register's address.
    std::uint32_t address{};

    /// The value read or written.
    std::uint32_t value{};
};

/// Receives the core's diagnostics: TTY text, POST codes and the device-access log.
///
/// A frontend implements it and passes it to the core when creating it, and it must outlive the
/// core. The core calls it synchronously during a run, as the guest acts, from the thread running
/// the core.
class Diagnostics {
  public:
    /// Destroys the sink.
    virtual ~Diagnostics() = default;

    /// Receives text the guest printed through the BIOS's putchar family.
    ///
    /// \param text The text, exactly as printed, line breaks included.
    virtual void tty_text(std::span<const char> text) = 0;

    /// Receives a POST code the guest wrote to the POST register in expansion region 2.
    ///
    /// \param code The code.
    virtual void post_code(std::uint8_t code) = 0;

    /// Receives the first read or the first write of a device register, in the guest's order.
    ///
    /// \param access The access.
    virtual void device_access(const DeviceAccess& access) = 0;

  protected:
    /// Creates the sink.
    Diagnostics() = default;

    /// Copies a sink, for a derived class's copy constructor.
    Diagnostics(const Diagnostics&) = default;

    /// Moves a sink, for a derived class's move constructor.
    Diagnostics(Diagnostics&&) = default;

    /// Copies a sink, for a derived class's copy assignment.
    Diagnostics& operator=(const Diagnostics&) = default;

    /// Moves a sink, for a derived class's move assignment.
    Diagnostics& operator=(Diagnostics&&) = default;
};

} // namespace coldboot
