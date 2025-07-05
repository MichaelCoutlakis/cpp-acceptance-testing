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

#include "cat/cat.h"
#include "cat/console_reporter.h"

class my_reporter : public cat::reporter
{
public:
    void on_assert(const cat::test_case &tc, const cat::assertion &a)
    {
        fmt::print(
            fmt::emphasis::bold |
                (a.get_result() ? fmt::fg(fmt::color::green) : fmt::fg(fmt::color::red)),
            "Test Case {} - Assert: {} : {}\n",
            tc.name(),
            a.m_expr,
            a.get_result());
    }
};

int main()
{
    my_reporter r;
    cat::console_reporter c;
    cat::register_reporter(&r);
    cat::register_reporter(&c);

    cat::test_runner runner;
    runner.run(cat::registry::get_instance().get_test_series_all());

    return 0;
}

TEST_CASE(TestCasePass)
{
    test(CAT_ASSERT(1 + 1 == 2));
    test(CAT_ASSERT(1 != 2));
    test(CAT_ASSERT(1 < 2));
    test(CAT_ASSERT(2 <= 2));
    test(CAT_ASSERT(4 >= 3));
    test(CAT_ASSERT(4 > 3));
    test(CAT_ASSERT(true && true));
}

TEST_CASE(TestCaseFail)
{
    test(CAT_ASSERT(1 + 1 == 3));
    test(CAT_ASSERT(1 != 1));
    test(CAT_ASSERT(3 < 2));
    test(CAT_ASSERT(4 <= 3));
    test(CAT_ASSERT(2 >= 3));
    test(CAT_ASSERT(2 > 3));
    test(CAT_ASSERT(true && false));
}
