// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// How Catch2 prints the core's values in a failed check.
///
/// Every test file whose checks hold one of these types includes this header. One that didn't
/// would print the value as `{?}`, and its copy of Catch2's printing could replace this one in
/// every other file of the program.

#pragma once

#include <coldboot/core/run_result.hpp>
#include <coldboot/core/stop_report.hpp>

#include <catch2/catch_tostring.hpp>

#include <string>

/// Prints a stop report as its line.
template <> struct Catch::StringMaker<coldboot::StopReport> {
    /// Converts the report.
    ///
    /// \param report The report.
    /// \return Its line.
    static std::string convert(const coldboot::StopReport& report) {
        const auto line = coldboot::stop_line(report);
        const auto text = line.text();
        return {text.begin(), text.end()};
    }
};

/// Prints why a run returned, with the stop report's line for a stop.
template <> struct Catch::StringMaker<coldboot::RunResult> {
    /// Converts the result.
    ///
    /// \param result The result.
    /// \return What it says.
    static std::string convert(const coldboot::RunResult& result) {
        std::string text = result.end == coldboot::RunEnd::stopped ? "stopped" : "reached until";
        if (result.stop) {
            text += ", " + StringMaker<coldboot::StopReport>::convert(*result.stop);
        }
        return text;
    }
};
