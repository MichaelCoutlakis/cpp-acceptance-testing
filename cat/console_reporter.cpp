/******************************************************************************
 * Copyright 2025 Michael Coutlakis
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include <fmt/color.h>
#include <fmt/format.h>

#include "console_reporter.h"

namespace cat
{
void console_reporter::on_test_starting(const test_case &tc)
{
    fmt::print(
        fmt::emphasis::bold | fmt::fg(fmt::color::green), "Running test case {}\n", tc.name());
}

void console_reporter::on_test_finished(const test_case &tc)
{
    const bool b = tc.get_result();
    fmt::print(
        b ? (fmt::emphasis::bold | fmt::fg(fmt::color::green))
          : (fmt::emphasis::bold | fmt::fg(fmt::color::red)),
        "Test Case {} {}\n",
        tc.name(),
        b ? "passed" : "failed");
}

void console_reporter::on_test_series_start(const test_series &ts)
{
    fmt::print(
        fmt::emphasis::bold | fmt::fg(fmt::color::green),
        "Starting test series {}\n",
        ts.get_name());
}

void console_reporter::on_test_series_end(const test_series &ts)
{
    fmt::print(
        fmt::emphasis::bold | fmt::fg(fmt::color::green),
        "Completed test series {}\n",
        ts.get_name());
}
}