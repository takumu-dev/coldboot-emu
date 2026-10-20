// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/text_writer.hpp>

#include "as_string.hpp"
#include <coldboot/core/literal.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>

using coldboot::test::as_string;

TEST_CASE("a text writer starts empty", "[core]") {
    const coldboot::TextWriter<8> writer;

    CHECK(writer.text().empty());
}

TEST_CASE("a text writer holds what is written to it, in order", "[core]") {
    coldboot::TextWriter<16> writer;

    writer.write(coldboot::literal("stop"));
    writer.write(coldboot::literal(": "));
    writer.write(coldboot::literal("here"));

    CHECK(as_string(writer.text()) == "stop: here");
}

TEST_CASE("a text writer may be filled to its capacity", "[core]") {
    coldboot::TextWriter<4> writer;

    writer.write(coldboot::literal("abcd"));

    CHECK(as_string(writer.text()) == "abcd");
}

TEST_CASE("decimal numbers are written without leading zeros", "[core]") {
    coldboot::TextWriter<64> writer;

    writer.write_decimal(0);
    writer.write(coldboot::literal(" "));
    writer.write_decimal(1'234);
    writer.write(coldboot::literal(" "));
    writer.write_decimal(std::numeric_limits<std::uint64_t>::max());

    CHECK(as_string(writer.text()) == "0 1234 18446744073709551615");
}

TEST_CASE("hexadecimal numbers are written in upper case, padded with zeros to their digits",
          "[core]") {
    coldboot::TextWriter<64> writer;

    writer.write_hex(0x1F80'2020, 8);
    writer.write(coldboot::literal(" "));
    writer.write_hex(0xA, 2);
    writer.write(coldboot::literal(" "));
    writer.write_hex(0, 1);
    writer.write(coldboot::literal(" "));
    writer.write_hex(0xFFFF'FFFF, 8);

    CHECK(as_string(writer.text()) == "1F802020 0A 0 FFFFFFFF");
}
