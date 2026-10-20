// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/core.hpp>

#include "core_printing.hpp" // NOLINT(misc-include-cleaner): Catch2 prints results with it
#include "r3000_encoder.hpp"
#include "recording_diagnostics.hpp"
#include "rom_builder.hpp"
#include "test_roms.hpp"
#include <coldboot/core/access.hpp>
#include <coldboot/core/core_config.hpp>
#include <coldboot/core/run_result.hpp>
#include <coldboot/core/stop_report.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>

using namespace coldboot::test;
using coldboot::AbsentPart;
using coldboot::AccessDirection;
using coldboot::AccessWidth;
using coldboot::Core;
using coldboot::RunEnd;
using coldboot::StopReport;

namespace {

// Far enough off that every test ROM stops first.
constexpr std::uint64_t distant = 1'000'000;

// A core created from `rom`, which it copies, reporting to `diagnostics`.
std::unique_ptr<Core> make_core(const Rom& rom, RecordingDiagnostics& diagnostics) {
    return std::make_unique<Core>(
        coldboot::CoreConfig{.bios = rom, .video_clock = {}, .diagnostics = diagnostics});
}

// The result of a run that stopped with `report`.
coldboot::RunResult stopped_with(const StopReport& report) {
    return {.end = RunEnd::stopped, .stop = report};
}

// The result of a run that reached its until.
constexpr coldboot::RunResult reached_until{.end = RunEnd::reached_until};

// How stop_rom() stops: its store to the DUART, the fourth instruction.
constexpr StopReport stop_rom_stop{
    .part = AbsentPart::unmapped,
    .address = 0x1F80'2020,
    .direction = AccessDirection::write,
    .width = AccessWidth::word,
    .pc = 0xBFC0'000C,
    .time = 3,
};

// A store of r0 to the DUART, by the instruction at `pc` at emulated time `time`.
constexpr StopReport duart_stop(std::uint32_t pc, std::uint64_t time) {
    return {
        .part = AbsentPart::unmapped,
        .address = 0x1F80'2020,
        .direction = AccessDirection::write,
        .width = AccessWidth::word,
        .pc = pc,
        .time = time,
    };
}

} // namespace

TEST_CASE("a core starts at the reset vector, with its registers and emulated time at zero",
          "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({}), diagnostics);

    const auto state = core->cpu_state();

    CHECK(state.pc == 0xBFC0'0000);
    CHECK(std::ranges::all_of(state.gpr, [](std::uint32_t value) { return value == 0; }));
    CHECK(core->now() == 0);
}

TEST_CASE("a run returns when emulated time reaches its until, one instruction a cycle", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({}), diagnostics);

    CHECK(core->run(10) == reached_until);
    CHECK(core->now() == 10);
    CHECK(core->cpu_state().pc == 0xBFC0'0028);
}

TEST_CASE("a run to one cycle past now runs exactly one instruction", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({}), diagnostics);
    static_cast<void>(core->run(5));

    CHECK(core->run(core->now() + 1) == reached_until);
    CHECK(core->now() == 6);
    CHECK(core->cpu_state().pc == 0xBFC0'0018);
}

TEST_CASE("a store to the absent DUART stops the core, reporting the access, PC and time",
          "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*stop_rom(), diagnostics);

    CHECK(core->run(distant) == stopped_with(stop_rom_stop));
}

TEST_CASE("a stop leaves the instruction that stopped undone, and emulated time where it began",
          "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*stop_rom(), diagnostics);

    static_cast<void>(core->run(distant));

    CHECK(core->now() == 3);
    const auto state = core->cpu_state();
    CHECK(state.pc == 0xBFC0'000C);
    CHECK(gpr(state, reg::t0) == 0x1F80'2000);
    CHECK(gpr(state, reg::t1) == 0x41);
}

TEST_CASE("a stop is final: every later run returns the same report at once", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*stop_rom(), diagnostics);
    static_cast<void>(core->run(distant));

    for (int run = 0; run < 3; ++run) {
        CHECK(core->run(core->now() + distant) == stopped_with(stop_rom_stop));
    }
    CHECK(core->now() == 3);
}

TEST_CASE("the core keeps its own copy of the BIOS", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto rom = stop_rom();
    const auto core = make_core(*rom, diagnostics);

    // Every word becomes FFFFFFFFh, an instruction the core doesn't decode.
    std::ranges::fill(*rom, std::byte{0xFF});

    CHECK(core->run(distant) == stopped_with(stop_rom_stop));
}

TEST_CASE("a stop on an absent part reports no diagnostics", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*stop_rom(), diagnostics);

    static_cast<void>(core->run(distant));

    CHECK(diagnostics.tty_text_received.empty());
    CHECK(diagnostics.post_codes_received.empty());
    CHECK(diagnostics.device_accesses_received.empty());
}

TEST_CASE("lui puts its immediate in a register's upper half and zeros in its lower", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    ori(reg::t0, reg::zero, 0xFFFF),
                                    lui(reg::t0, 0x1234),
                                }),
                                diagnostics);

    static_cast<void>(core->run(2));

    CHECK(gpr(core->cpu_state(), reg::t0) == 0x1234'0000);
}

TEST_CASE("ori ORs a register with its zero-extended immediate", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    lui(reg::t0, 0xF0F0),
                                    ori(reg::t1, reg::t0, 0x8001),
                                    ori(reg::t2, reg::zero, 0x8000),
                                }),
                                diagnostics);

    static_cast<void>(core->run(3));

    const auto state = core->cpu_state();
    CHECK(gpr(state, reg::t1) == 0xF0F0'8001);
    CHECK(gpr(state, reg::t2) == 0x0000'8000);
}

TEST_CASE("addiu adds its sign-extended immediate, wrapping without a trap", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    addiu(reg::t0, reg::zero, -1),
                                    addiu(reg::t1, reg::t0, 1),
                                    lui(reg::t2, 0x7FFF),
                                    ori(reg::t2, reg::t2, 0xFFFF),
                                    addiu(reg::t2, reg::t2, 1),
                                    addiu(reg::t3, reg::zero, -32768),
                                }),
                                diagnostics);

    static_cast<void>(core->run(6));

    const auto state = core->cpu_state();
    CHECK(gpr(state, reg::t0) == 0xFFFF'FFFF);
    CHECK(gpr(state, reg::t1) == 0);
    CHECK(gpr(state, reg::t2) == 0x8000'0000);
    CHECK(gpr(state, reg::t3) == 0xFFFF'8000);
}

TEST_CASE("sll shifts a register left by its amount, dropping the bits shifted out", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    ori(reg::t0, reg::zero, 0x8001),
                                    sll(reg::t1, reg::t0, 16),
                                    sll(reg::t2, reg::t0, 31),
                                    sll(reg::t3, reg::t0, 0),
                                }),
                                diagnostics);

    static_cast<void>(core->run(4));

    const auto state = core->cpu_state();
    CHECK(gpr(state, reg::t1) == 0x8001'0000);
    CHECK(gpr(state, reg::t2) == 0x8000'0000);
    CHECK(gpr(state, reg::t3) == 0x0000'8001);
}

TEST_CASE("r0 reads 0 whatever is written to it", "[core]") {
    RecordingDiagnostics diagnostics;
    // Each instruction writes r0 with something other than 0, and the last copies it to t1.
    const auto core = make_core(*build_rom({
                                    ori(reg::t0, reg::zero, 1),
                                    sll(reg::zero, reg::t0, 4),
                                    addiu(reg::zero, reg::zero, 5),
                                    ori(reg::zero, reg::zero, 0xFFFF),
                                    lui(reg::zero, 0xFFFF),
                                    addiu(reg::t1, reg::zero, 0),
                                }),
                                diagnostics);

    static_cast<void>(core->run(6));

    const auto state = core->cpu_state();
    CHECK(gpr(state, reg::zero) == 0);
    CHECK(gpr(state, reg::t1) == 0);
}

TEST_CASE("a store to RAM carries on, through KUSEG, KSEG0 and KSEG1", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    sw(reg::zero, 0, reg::zero),
                                    lui(reg::t0, 0x8000),
                                    sw(reg::zero, 0x10, reg::t0),
                                    lui(reg::t0, 0xA020),
                                    sw(reg::zero, -4, reg::t0),
                                    lui(reg::t0, 0x1F80),
                                    sw(reg::zero, 0x2020, reg::t0),
                                }),
                                diagnostics);

    CHECK(core->run(distant) == stopped_with(duart_stop(0xBFC0'0018, 6)));
}

TEST_CASE("a store to the ROM leaves it as it was", "[core]") {
    RecordingDiagnostics diagnostics;
    // The store aims a nop at the instruction after it, which still stores to the DUART.
    const auto core = make_core(*build_rom({
                                    lui(reg::t0, 0xBFC0),
                                    lui(reg::t1, 0x1F80),
                                    sw(reg::zero, 0x0C, reg::t0),
                                    sw(reg::zero, 0x2020, reg::t1),
                                }),
                                diagnostics);

    CHECK(core->run(distant) == stopped_with(duart_stop(0xBFC0'000C, 3)));
}

TEST_CASE("an address that maps to nothing stops the core: above KUSEG's first 512 MiB, and KSEG2",
          "[core]") {
    const auto base = GENERATE(as<std::uint16_t>{}, 0x2000, 0x7FFF, 0xC000, 0xFFFF);
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    lui(reg::t0, base),
                                    sw(reg::zero, 0, reg::t0),
                                }),
                                diagnostics);

    CHECK(core->run(distant) == stopped_with({
                                    .part = AbsentPart::unmapped,
                                    .address = std::uint32_t{base} << 16,
                                    .direction = AccessDirection::write,
                                    .width = AccessWidth::word,
                                    .pc = 0xBFC0'0004,
                                    .time = 1,
                                }));
}

TEST_CASE("a store's address wraps past the top of the address space", "[core]") {
    RecordingDiagnostics diagnostics;
    // FFFFFFFCh + 8 wraps to 4, in RAM.
    const auto core = make_core(*build_rom({
                                    addiu(reg::t0, reg::zero, -4),
                                    sw(reg::zero, 8, reg::t0),
                                    lui(reg::t1, 0x1F80),
                                    sw(reg::zero, 0x2020, reg::t1),
                                }),
                                diagnostics);

    CHECK(core->run(distant) == stopped_with(duart_stop(0xBFC0'000C, 3)));
}

TEST_CASE("an instruction the core doesn't decode stops it at that instruction", "[core]") {
    const auto word = GENERATE(as<std::uint32_t>{}, 0xFFFF'FFFF, 0x0000'0020);
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({nop(), word}), diagnostics);

    CHECK(core->run(distant) == stopped_with({
                                    .part = AbsentPart::instruction,
                                    .address = 0xBFC0'0004,
                                    .direction = AccessDirection::read,
                                    .width = AccessWidth::word,
                                    .pc = 0xBFC0'0004,
                                    .time = 1,
                                }));
}

TEST_CASE("a store to an address that isn't a multiple of 4 stops the core with an address error",
          "[core]") {
    // Alignment is checked before the address is mapped, so the DUART's address is no different.
    const auto base = GENERATE(as<std::uint16_t>{}, 0x8000, 0x1F80);
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({
                                    lui(reg::t0, base),
                                    sw(reg::zero, 0x2022, reg::t0),
                                }),
                                diagnostics);

    CHECK(core->run(distant) == stopped_with({
                                    .part = AbsentPart::address_error,
                                    .address = (std::uint32_t{base} << 16) + 0x2022,
                                    .direction = AccessDirection::write,
                                    .width = AccessWidth::word,
                                    .pc = 0xBFC0'0004,
                                    .time = 1,
                                }));
}

TEST_CASE("a PC run past the end of the ROM stops the core at the fetch", "[core]") {
    RecordingDiagnostics diagnostics;
    const auto core = make_core(*build_rom({}), diagnostics);

    CHECK(core->run(distant) == stopped_with({
                                    .part = AbsentPart::unmapped,
                                    .address = 0xBFC8'0000,
                                    .direction = AccessDirection::read,
                                    .width = AccessWidth::word,
                                    .pc = 0xBFC8'0000,
                                    .time = 131'072,
                                }));
}
