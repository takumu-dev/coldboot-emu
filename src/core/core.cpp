// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/core.hpp>

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/core_config.hpp>
#include <coldboot/core/cpu_state.hpp>
#include <coldboot/core/run_result.hpp>

#include <cstdint>

namespace coldboot {

Core::Core(const CoreConfig& config) : bus_{config.bios} {}

RunResult Core::run(std::uint64_t until) {
    COLDBOOT_ASSERT(until > now_, "a run ends later than it starts");
    while (!stop_ && now_ < until) {
        stop_ = cpu_.step(bus_, now_);
        if (!stop_) {
            ++now_;
        }
    }
    if (stop_) {
        return {.end = RunEnd::stopped, .stop = stop_};
    }
    return {.end = RunEnd::reached_until};
}

std::uint64_t Core::now() const { return now_; }

CpuState Core::cpu_state() const { return cpu_.state(); }

} // namespace coldboot
