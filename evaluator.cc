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
evaluator::visit(node &n, const declaration &)
{
    //const auto var = std::get<variable_ref>(n.children[0]->kind_).variable_;
    //symbol_scope::add(var);
}

void
evaluator::visit(node &n, const compound_statement&)
{
    symbol_scope locals;
    auto &c = n.children;
    for (const auto &child : c) {
        this->accept(*child);
    }
}

void
evaluator::visit(node &n, const if_statement &)
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
evaluator::visit(node &n, const assignment_statement &)
{
    accept(*n.children[1]);
    //auto var = std::get<variable_ref>(n.children[0]->kind_).variable;
    //checkKeyword(var);
    //symbol_scope::lookup(var) = result_;
    //std::cerr << "Result: " << var << " = " << result_ << std::endl;
}

void
evaluator::visit(node &n, const expression_statement &)
{
    accept(*n.children[0]);
    std::cerr << "Result: " << result_ << std::endl;
}

void
evaluator::visit(node &n, const variable_ref &sym)
{
    //const auto &var = sym.variable;
    //set_result(symbol_scope::lookup(var));
}

void
evaluator::visit(node &, const number &i)
{
    set_result(i.value_);
}

void
evaluator::visit(node &n, const unary_minus &)
{
    accept(*n.children[0]);
    set_result(-1 * result_);
}

void
evaluator::visit(node &n, const unary_plus &)
{
    accept(*n.children[0]);
}

void
evaluator::visit(node &n, const multiplication &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs * rhs);
}

void
evaluator::visit(node &n, const division &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs / rhs);
}

void
evaluator::visit(node &n, const modulus &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs % rhs);
}

void
evaluator::visit(node &n, const addition &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs + rhs);
}

void
evaluator::visit(node &n, const subtraction &)
{
    accept(*n.children[0]);
    auto lhs = result_;
    accept(*n.children[1]);
    auto rhs = result_;
    set_result(lhs - rhs);
}

void
evaluator::visit(node &n, const logical_or &)
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
evaluator::visit(node &n, const logical_or_else &)
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
evaluator::visit(node &n, const logical_and &)
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
evaluator::visit(node &n, const logical_and_then &)
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
evaluator::visit(node &n, const equal_to &)
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
evaluator::visit(node &n, const not_equal &)
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
evaluator::visit(node &n, const less_than &)
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
evaluator::visit(node &n, const less_or_equal &)
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
evaluator::visit(node &n, const greater_than &)
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
evaluator::visit(node &n, const greater_or_equal &)
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
evaluator::visit(node &n, const function_call &f)
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
