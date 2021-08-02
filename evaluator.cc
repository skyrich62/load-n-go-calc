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

namespace Calc {

using namespace Calc::Node;

void
evaluator::visit(node &n, declaration &)
{
}

void
evaluator::visit(node &n, variable&)
{
}

void
evaluator::visit(node &n, root &)
{
    auto &c = n.children;
    for (const auto &child : c) {
        this->accept(*child);
    }
}

void
evaluator::visit(node &n, variable_ref &var)
{
    auto ptr = var.variable_;
    set_result(values_[ptr]);
}

void
evaluator::visit(node &n, scope &)
{
}

void
evaluator::visit(node &n, compound_statement&)
{
    auto &c = n.children;
    for (const auto &child : c) {
        this->accept(*child);
    }
}

void
evaluator::visit(node &n, if_statement &)
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
evaluator::visit(node &n, assignment_statement &)
{
    accept(*n.children[1]);
    auto var = std::get<variable_ref>(n.children[0]->kind_).variable_;
    auto name = std::get<variable>(var->kind_).name_;
    values_[var] = result_;
    std::cerr << "Result: " << name << " = " << result_ << std::endl;
}

void
evaluator::visit(node &n, expression_statement &)
{
    accept(*n.children[0]);
    std::cerr << "Result: " << result_ << std::endl;
}

void
evaluator::visit(node &, number &i)
{
    set_result(i.value_);
}

void
evaluator::visit(node &n, unary_minus &)
{
    accept(*n.children[0]);
    set_result(-1 * result_);
}

void
evaluator::visit(node &n, unary_plus &)
{
    accept(*n.children[0]);
}

void
evaluator::visit(node &n, multiplication &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs * rhs);
}

void
evaluator::visit(node &n, division &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs / rhs);
}

void
evaluator::visit(node &n, modulus &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs % rhs);
}

void
evaluator::visit(node &n, addition &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs + rhs);
}

void
evaluator::visit(node &n, subtraction &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs - rhs);
}

void
evaluator::visit(node &n, logical_or &)
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
evaluator::visit(node &n, logical_or_else &)
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
evaluator::visit(node &n, logical_and &)
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
evaluator::visit(node &n, logical_and_then &)
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
evaluator::visit(node &n, equal_to &)
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
evaluator::visit(node &n, not_equal &)
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
evaluator::visit(node &n, less_than &)
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
evaluator::visit(node &n, less_or_equal &)
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
evaluator::visit(node &n, greater_than &)
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
evaluator::visit(node &n, greater_or_equal &)
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
evaluator::visit(node &n, function_call &f)
{
    /*
    accept(*n.children[1]);
    auto operand = result_;
    auto func = std::get<variable_ref>(n.children[0]->kind_).variable;
    if (func == "abs") {
        set_result(abs(operand));
    } else if (func == "sgn") {
        if (operand < 0) {
            set_result(-1);
            return;
        } else if (operand > 0) {
            set_result(1);
            return;
        }
        set_result(0);
    }
    std::cerr << "Unknown function: " << func << " -- ignored.\n";
    set_result(operand);
    */
}

} // namespace Calc
