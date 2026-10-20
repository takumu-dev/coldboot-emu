// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/stop_report.hpp>

#include <coldboot/core/access.hpp>
#include <coldboot/core/literal.hpp>

namespace coldboot {

namespace {

void write_part(Line& line, AbsentPart part) {
    switch (part) {
    case AbsentPart::unmapped:
        line.write(literal("nothing"));
        return;
    case AbsentPart::instruction:
        line.write(literal("instruction"));
        return;
    case AbsentPart::address_error:
        line.write(literal("address error"));
        return;
    }
}

void write_access(Line& line, AccessDirection direction, AccessWidth width) {
    switch (width) {
    case AccessWidth::byte:
        line.write(literal("8-bit"));
        break;
    case AccessWidth::halfword:
        line.write(literal("16-bit"));
        break;
    case AccessWidth::word:
        line.write(literal("32-bit"));
        break;
    }
    switch (direction) {
    case AccessDirection::read:
        line.write(literal(" read"));
        return;
    case AccessDirection::write:
        line.write(literal(" write"));
        return;
    }
}

} // namespace

Line stop_line(const StopReport& report) {
    Line line;
    line.write(literal("stop: "));
    write_part(line, report.part);
    line.write(literal(" at "));
    line.write_hex(report.address, 8);
    line.write(literal("h, "));
    write_access(line, report.direction, report.width);
    line.write(literal(", PC "));
    line.write_hex(report.pc, 8);
    line.write(literal("h, emulated time "));
    line.write_decimal(report.time);
    return line;
}

} // namespace coldboot
