// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/detail/cpu.hpp>

#include <coldboot/core/access.hpp>
#include <coldboot/core/cpu_state.hpp>
#include <coldboot/core/detail/bus.hpp>
#include <coldboot/core/stop_report.hpp>

#include <array>
#include <cstdint>
#include <optional>

namespace coldboot::detail {

namespace {

// The fields of an instruction word, named and laid out as psx-spx's "CPU Opcode Encoding" and the
// R3000 manual give them.

std::uint32_t op(std::uint32_t word) { return word >> 26; }

std::uint32_t rs(std::uint32_t word) { return word >> 21 & 0x1F; }

std::uint32_t rt(std::uint32_t word) { return word >> 16 & 0x1F; }

std::uint32_t rd(std::uint32_t word) { return word >> 11 & 0x1F; }

std::uint32_t shamt(std::uint32_t word) { return word >> 6 & 0x1F; }

std::uint32_t funct(std::uint32_t word) { return word & 0x3F; }

std::uint32_t immediate(std::uint32_t word) { return word & 0xFFFF; }

std::uint32_t sign_extended_immediate(std::uint32_t word) {
    return static_cast<std::uint32_t>(static_cast<std::int16_t>(word & 0xFFFF));
}

// The values of op that the CPU decodes, from psx-spx's "CPU Opcode Encoding".
namespace opcode {
constexpr std::uint32_t special = 0x00;
constexpr std::uint32_t addiu = 0x09;
constexpr std::uint32_t ori = 0x0D;
constexpr std::uint32_t lui = 0x0F;
constexpr std::uint32_t sw = 0x2B;
} // namespace opcode

// The values of funct, under SPECIAL, that the CPU decodes.
namespace function {
constexpr std::uint32_t sll = 0x00;
} // namespace function

// Writes a general-purpose register. r0 is wired to 0, so a write to it is dropped.
void write_gpr(std::array<std::uint32_t, 32>& gpr, std::uint32_t index, std::uint32_t value) {
    if (index != 0) {
        gpr[index] = value;
    }
}

// The report of a word access to `address`, by the instruction at `pc`, that met the absent
// `part`. It gives the address in the physical address space, so that a part reached through any
// segment is reported at the same address.
StopReport stop_report(AbsentPart part, std::uint32_t address, AccessDirection direction,
                       std::uint32_t pc, std::uint64_t now) {
    return {
        .part = part,
        .address = physical_address(address),
        .direction = direction,
        .width = AccessWidth::word,
        .pc = pc,
        .time = now,
    };
}

} // namespace

std::optional<StopReport> Cpu::step(Bus& bus, std::uint64_t now) {
    const auto fetched = bus.read_word(pc_);
    if (!fetched) {
        return stop_report(AbsentPart::unmapped, pc_, AccessDirection::read, pc_, now);
    }
    const std::uint32_t word = *fetched;

    switch (op(word)) {
    case opcode::addiu:
        // ADDIU wraps, and has no overflow trap.
        write_gpr(gpr_, rt(word), gpr_[rs(word)] + sign_extended_immediate(word));
        break;
    case opcode::ori:
        write_gpr(gpr_, rt(word), gpr_[rs(word)] | immediate(word));
        break;
    case opcode::lui:
        write_gpr(gpr_, rt(word), immediate(word) << 16);
        break;
    case opcode::sw: {
        // The address wraps, as the R3000A's does.
        const std::uint32_t address = gpr_[rs(word)] + sign_extended_immediate(word);
        if (address % 4 != 0) {
            return stop_report(AbsentPart::address_error, address, AccessDirection::write, pc_,
                               now);
        }
        if (!bus.write_word(address, gpr_[rt(word)])) {
            return stop_report(AbsentPart::unmapped, address, AccessDirection::write, pc_, now);
        }
        break;
    }
    case opcode::special:
        if (funct(word) == function::sll) {
            write_gpr(gpr_, rd(word), gpr_[rt(word)] << shamt(word));
            break;
        }
        // Any other function of SPECIAL is an instruction the CPU doesn't decode.
        [[fallthrough]];
    default:
        return stop_report(AbsentPart::instruction, pc_, AccessDirection::read, pc_, now);
    }
    pc_ += 4;
    return std::nullopt;
}

CpuState Cpu::state() const { return {.gpr = gpr_, .pc = pc_}; }

} // namespace coldboot::detail
