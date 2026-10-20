// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// A diagnostics sink that keeps everything the core reports, for tests to check.

#pragma once

#include <coldboot/core/diagnostics.hpp>

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace coldboot::test {

/// Keeps everything the core reports, in the order it arrives.
class RecordingDiagnostics final : public Diagnostics {
  public:
    /// The TTY text received, joined.
    std::string tty_text_received;

    /// The POST codes received.
    std::vector<std::uint8_t> post_codes_received;

    /// The device accesses received.
    std::vector<DeviceAccess> device_accesses_received;

    /// Keeps TTY text.
    ///
    /// \param text The text.
    void tty_text(std::span<const char> text) override {
        tty_text_received.append(text.begin(), text.end());
    }

    /// Keeps a POST code.
    ///
    /// \param code The code.
    void post_code(std::uint8_t code) override { post_codes_received.push_back(code); }

    /// Keeps a device access.
    ///
    /// \param access The access.
    void device_access(const DeviceAccess& access) override {
        device_accesses_received.push_back(access);
    }
};

} // namespace coldboot::test
