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

namespace cat
{

// https://fekir.info/post/decomposing-an-expression/
enum class op
{
    none = 0,    // Uninitialised
    eq,          // LHS == RHS
    neq,         // LHS != RHS
    le,          // LHS <= RHS
    l,           // LHS <  RHS
    ge,          // LHS >= RHS
    g,           // LHS >  RHS
    logical_and, // LHS && RHS
};

struct assertion
{
    const char *m_expr;
    op m_op;
    bool m_result{false};
    std::string m_lhs;
    std::string m_rhs;

    bool get_result() const { return m_result; }
};

template <class LHS>
struct assertion_builder
{
    LHS m_lhs;              //!< The left hand side gets captured
    assertion &m_assertion; //!< Build the assert data here

    // clang-format off
    template <class RHS>
    assertion operator==(RHS &&rhs) && { return set(op::eq, m_lhs == rhs); }

    template <class RHS>
    assertion operator!=(RHS &&rhs) && { return set(op::neq, m_lhs != rhs); }

    template <class RHS>
    assertion operator<(RHS &&rhs) && { return set(op::l, m_lhs < rhs); }

    template <class RHS>
    assertion operator<=(RHS &&rhs) && { return set(op::le, m_lhs <= rhs); }

    template <class RHS>
    assertion operator>(RHS &&rhs) && { return set(op::g, m_lhs > rhs); }

    template <class RHS>
    assertion operator>=(RHS &&rhs) { return set(op::ge, m_lhs >= rhs); }

    template <class RHS>
    assertion operator&&(RHS &&rhs) && { return set(op::logical_and, m_lhs && rhs); }

    // clang-format on

    assertion &&set(op the_op, bool result)
    {
        m_assertion.m_op = the_op;
        m_assertion.m_result = result;
        return std::move(m_assertion);
    }
};

struct decompose_helper
{
    assertion m_assertion;

    template <class LHS>
    assertion_builder<LHS> operator<(LHS &&lhs) &&
    {
        return assertion_builder<LHS>{std::forward<LHS>(lhs), m_assertion};
    }
};

/// Decompose the binary comparison expression using the operator precedence of <.
/// See https://fekir.info/post/decomposing-an-expression/
#define DECOMPOSE_BINARY_OP(expr)                                                                  \
    cat::decompose_helper{cat::assertion{#expr, cat::op::none, false}} < expr

#define CAT_ASSERT(expr) DECOMPOSE_BINARY_OP(expr)
}