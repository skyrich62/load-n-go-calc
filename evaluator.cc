/**
 * @copyright
 * Copyright (c) 2021 Rich Newman
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author
 * Rich Newman
 */

#include "evaluator.h"
#include <iostream>
#include <set>

#include <CompuBrite/CheckPoint.h>

namespace Calc {
namespace cbi = CompuBrite;

using namespace Calc::Node;

void
evaluator::pre_visit(node &n, declaration &)
{
}

void
evaluator::pre_visit(node &n, variable&)
{
}

void
evaluator::pre_visit(node &n, function&)
{
}

void
evaluator::pre_visit(node &n, root &)
{
    auto &c = n.children;
    for (const auto &child : c) {
        // catch misplaced exit or return statements... This should be caught
        // during semantic analysis, but that's for another patch.
        this->accept(*child);
    }
}

void
evaluator::pre_visit(node &n, variable_ref &var)
{
    auto ptr = var.symbol_;
    set_result(values_[ptr]);
}

void
evaluator::pre_visit(node &n, scope &)
{
}

void
evaluator::pre_visit(node &n, compound_statement&)
{
    auto &c = n.children;
    for (const auto &child : c) {
        this->accept(*child);
    }
}

void
evaluator::pre_visit(node &n, loop_top_test_statement &)
{
    auto &cond = *n.children[0];
    auto &body = *n.children[1];
    while (true) {
        accept(cond);
        if (result_ == 0) {
            return;
        }
        try {
            accept(body);
        } catch (const loop_exiting &e) {
            auto b = body.get_kind<compound_statement>();
            cbi::CheckPoint::expect(CBI_HERE, b, "Must be compound statement");
            if (!e.name_.empty() && e.name_ != b->name_) {
                throw;
            }
            return;
        }
    }
}

void
evaluator::pre_visit(node &n, loop_bottom_test_statement &)
{
    auto &cond = *n.children[1];
    auto &body = *n.children[0];
    do {
        try {
            accept(body);
        } catch (const loop_exiting &e) {
            auto b = body.get_kind<compound_statement>();
            cbi::CheckPoint::expect(CBI_HERE, b, "Must be compound statement");
            if (!e.name_.empty() && e.name_ != b->name_) {
                throw;
            }
            return;
        }
        accept(cond);
        if (result_ == 0) {
            return;
        }
    } while (true);
}

void
evaluator::pre_visit(node &n, if_statement &)
{
    auto &cond = *n.children[0];
    accept(cond);
    if (result_ != 0) {
        accept(*n.children[1]);
    } else {
        if (n.children.size() == 3) {
            accept(*n.children[2]);
        }
    }
}

void
evaluator::pre_visit(node &n, exit_statement &es)
{
    if (n.children.size() == 1) {
        auto &cond = *n.children[0];
        accept(cond);
        if (result_) {
            throw loop_exiting{es.name_};
        }
    } else {
        throw loop_exiting{es.name_};
    }
}

void
evaluator::pre_visit(node &n, return_statement &)
{
    accept(*n.children[0]);
    throw function_returning{};
}

void
evaluator::pre_visit(node &n, assignment_statement &)
{
    accept(*n.children[1]);
    auto var = n.children[0]->get_kind<variable_ref>()->symbol_;
    auto name = var->get_kind<variable>()->name_;
    values_[var] = result_;
    std::cerr << "Result: " << name << " = " << result_ << std::endl;
}

void
evaluator::pre_visit(node &n, expression_statement &)
{
    accept(*n.children[0]);
    std::cerr << "Result: " << result_ << std::endl;
}

void
evaluator::pre_visit(node &, number &i)
{
    set_result(i.value_);
}

void
evaluator::pre_visit(node &n, unary_minus &)
{
    accept(*n.children[0]);
    set_result(-1 * result_);
}

void
evaluator::pre_visit(node &n, unary_plus &)
{
    accept(*n.children[0]);
}

void
evaluator::pre_visit(node &n, multiplication &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs * rhs);
}

void
evaluator::pre_visit(node &n, division &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs / rhs);
}

void
evaluator::pre_visit(node &n, modulus &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs % rhs);
}

void
evaluator::pre_visit(node &n, addition &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs + rhs);
}

void
evaluator::pre_visit(node &n, subtraction &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs - rhs);
}

void
evaluator::pre_visit(node &n, logical_or &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if ((lhs != 0) || (rhs != 0)) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, logical_or_else &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    if (lhs != 0) {
        set_result(1);
        return;
    }
    accept(*n.children[1]);
    auto rhs = result_;
    if (rhs != 0) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, logical_not &)
{
    accept(*n.children[0]);
    auto operand = result_;
    set_result(!operand);
}

void
evaluator::pre_visit(node &n, logical_and &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if ((lhs != 0) && (rhs != 0)) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, logical_and_then &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    if (lhs == 0) {
        set_result(0);
        return;
    }
    accept(*n.children[1]);
    auto rhs = result_;
    if (rhs != 0) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, equal_to &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs == rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, not_equal &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs != rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, less_than &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs < rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, less_or_equal &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs <= rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, greater_than &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs > rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, greater_or_equal &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    if (lhs >= rhs) {
        set_result(1);
        return;
    }
    set_result(0);
}

void
evaluator::pre_visit(node &n, function_call &fc)
{
    cbi::CheckPoint::expect(CBI_HERE, fc.symbol_, "No defined function!");
    auto func_node = fc.symbol_->get_kind<function>();
    cbi::CheckPoint::expect(CBI_HERE, func_node, "func_node should not be null");

    if (auto func = func_node->get_intrinsic(); func) {
        accept(*n.children[0]);
        auto operand = result_;
        set_result(func(operand));
        return;
    }
    // Not an intrinsic function.
    // Evaluate each argument and assign it's value to the corresponding
    // parameter.  Recursive functions are not yet supported.
    cbi::CheckPoint cp("ev-function-call");
    cp.print(CBI_HERE, "function call: ", func_node->name_);
    auto &scope = func_node->scope_;
    auto param = scope->children.begin();
    for (auto &arg : n.children) {
        accept(*arg);
        //auto var = (*param)->get_kind<variable>();
        //auto name = var->get_kind<variable>()->name_;
        values_[(*param).get()] = result_;
        cp.print(CBI_HERE, "Param: ", result_);
        ++param;
    }
    auto &body = fc.symbol_->children[0];
    try {
        accept(*body);
    } catch (const function_returning &) {
        return;
    }
}

} // namespace Calc
