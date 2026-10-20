// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/stop_report.hpp>

#include <coldboot/core/access.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <span>
#include <string>

namespace {

using coldboot::AbsentPart;
using coldboot::AccessDirection;
using coldboot::AccessWidth;
using coldboot::StopReport;

std::string line_of(const StopReport& report) {
    const auto line = coldboot::stop_line(report);
    const std::span<const char> text = line.text();
    return {text.begin(), text.end()};
}

} // namespace

TEST_CASE("a stop's line names what was touched and where, the access, the PC and the time",
          "[core]") {
    const StopReport report{
        .part = AbsentPart::unmapped,
        .address = 0x1F80'2020,
        .direction = AccessDirection::write,
        .width = AccessWidth::word,
        .pc = 0xBFC0'000C,
        .time = 3,
    };

    CHECK(line_of(report) ==
          "stop: nothing at 1F802020h, 32-bit write, PC BFC0000Ch, emulated time 3");
}

TEST_CASE("a stop's line names a read, and each width of access", "[core]") {
    StopReport report{
        .part = AbsentPart::unmapped,
        .address = 0x0000'0001,
        .direction = AccessDirection::read,
        .width = AccessWidth::byte,
        .pc = 0x8000'0000,
        .time = 0,
    };
    CHECK(line_of(report) ==
          "stop: nothing at 00000001h, 8-bit read, PC 80000000h, emulated time 0");

    report.width = AccessWidth::halfword;
    CHECK(line_of(report) ==
          "stop: nothing at 00000001h, 16-bit read, PC 80000000h, emulated time 0");
}

TEST_CASE("a stop's line names an instruction the core doesn't decode", "[core]") {
    const StopReport report{
        .part = AbsentPart::instruction,
        .address = 0xBFC0'0010,
        .direction = AccessDirection::read,
        .width = AccessWidth::word,
        .pc = 0xBFC0'0010,
        .time = 4,
    };

    CHECK(line_of(report) ==
          "stop: instruction at BFC00010h, 32-bit read, PC BFC00010h, emulated time 4");
}

TEST_CASE("a stop's line names an address error", "[core]") {
    const StopReport report{
        .part = AbsentPart::address_error,
        .address = 0x8000'0002,
        .direction = AccessDirection::write,
        .width = AccessWidth::word,
        .pc = 0xBFC0'0008,
        .time = 2,
    };

    CHECK(line_of(report) ==
          "stop: address error at 80000002h, 32-bit write, PC BFC00008h, emulated time 2");
}

TEST_CASE("a stop's line holds the longest report", "[core]") {
    const StopReport report{
        .part = AbsentPart::address_error,
        .address = 0xFFFF'FFFF,
        .direction = AccessDirection::write,
        .width = AccessWidth::halfword,
        .pc = 0xFFFF'FFFF,
        .time = std::numeric_limits<std::uint64_t>::max(),
    };

    CHECK(line_of(report) == "stop: address error at FFFFFFFFh, 16-bit write, PC FFFFFFFFh, "
                             "emulated time 18446744073709551615");
}
