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
#pragma once
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "cat/assertions.h"

namespace cat
{

struct test_case;
struct test_series;

struct reporter
{
    virtual ~reporter() = default;
    virtual void on_assert(const test_case &tc, const assertion &a) { }
    virtual void on_test_starting(const test_case &tc) { }
    virtual void on_test_finished(const test_case &tc) { }
    virtual void on_test_series_start(const test_series &ts) { }
    virtual void on_test_series_end(const test_series &ts) { }
};

inline std::vector<reporter *> &get_reporters()
{
    static std::vector<reporter *> reporters;
    return reporters;
}

void register_reporter(reporter *r);

struct test_case
{
    virtual ~test_case() = default;

    virtual void run() = 0;
    virtual std::string name() const = 0;

    template <typename AssertionType>
    bool test(AssertionType &&a)
    {
        const bool result = a.get_result();
        m_result = m_result.value_or(true) && result;
        auto p = std::make_unique<std::decay_t<AssertionType>>(std::forward<AssertionType>(a));

        for(auto &r : get_reporters())
            r->on_assert(std::as_const(*this), *p);

        return result;
    }
    bool get_result() const { return m_result.value(); }

private:
    std::optional<bool> m_result;
};

class test_series
{
public:
    test_series(std::string name, const std::vector<std::string> tests) :
        m_name(name), m_tests(tests)
    {
    }

    std::string get_name() const { return m_name; }
    const std::vector<std::string> &get_tests() const { return m_tests; }

private:
    std::string m_name;
    std::vector<std::string> m_tests;
};

using test_case_factory = std::function<std::unique_ptr<test_case>()>;

class registry
{
public:
    static registry &get_instance()
    {
        static registry instance;
        return instance;
    }

    void register_test_case(std::string name, test_case_factory factory)
    {
        auto [it, was_inserted] = m_registry.emplace(std::move(name), std::move(factory));
        if(!was_inserted)
            throw std::runtime_error("Duplicate test case factory: " + it->first);
    }

    std::unique_ptr<test_case> load_test_case(const std::string &name)
    {
        // Create an object of the test case using the factory function.
        // If it hasn't been registered yet, that should be an exception.
        return m_registry.at(name)();
    }

    test_series get_test_series_all()
    {
        std::vector<std::string> names;
        for(auto &[key, val] : m_registry)
            names.push_back(key);
        return test_series("all", names);
    }

private:
    registry() = default;
    std::map<std::string, test_case_factory> m_registry;
};

class test_runner
{
public:
    void run(const test_series &ts)
    {
        for(auto &r : get_reporters())
            r->on_test_series_start(ts);

        for(auto &test_case_name : ts.get_tests())
        {
            auto tc = registry::get_instance().load_test_case(test_case_name);

            for(auto &r : get_reporters())
                r->on_test_starting(*tc);

            tc->run();

            for(auto &r : get_reporters())
                r->on_test_finished(*tc);
        }
        for(auto &r : get_reporters())
            r->on_test_series_end(ts);
    }
};

#define TEST_CASE(Name)                                                                            \
    struct test_case_##Name : public ::cat::test_case                                              \
    {                                                                                              \
        std::string name() const override { return #Name; }                                        \
        void run() override;                                                                       \
                                                                                                   \
    private:                                                                                       \
        static bool m_registered;                                                                  \
    };                                                                                             \
    bool test_case_##Name## ::m_registered = []                                                    \
    {                                                                                              \
        ::cat::registry::get_instance().register_test_case(                                        \
            #Name, []() { return std::make_unique<test_case_##Name##>(); });                       \
        return true;                                                                               \
    }();                                                                                           \
    void test_case_##Name::run()
}
