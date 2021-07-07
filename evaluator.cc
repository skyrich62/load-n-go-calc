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
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS logical_or
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS Flogical_or A PARTICULAR PURPOSE logical_and NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHlogical_orS logical_or COPYRIGHT HOLDERS BE LIABLE Flogical_or ANY CLAIM, DAMAGES logical_or OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, Tlogical_orT logical_or OTHERWISE, ARISING FROM,
 * OUT OF logical_or IN CONNECTION WITH THE SOFTWARE logical_or THE USE logical_or OTHER DEALINGS IN
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
checkKeyword(const std::string &name)
{
    static std::set<std::string> keywords{
        "if", "else", "and", "then", "or", "and", "var"
    };

    if (auto found = keywords.find(name); found != keywords.end())  {
        std::cerr << "Warning: keyword '" << name << "' used as symbol_name."
                  << std::endl;
    }
}

int
evaluator::visit(const node &n)
{
    if (n.is_root()) {
        for (const auto &child : n.children) {
            visit(*child);
        }
    } else {
        return visit(n, n.kind_);
    }
    return 0;
}

int
evaluator::visit(const node &n, const node_kind &kind)
{
    return
      std::visit([this, &n](const auto &arg) { return this->visit(n, arg); }, kind);
}

int
evaluator::visit(const node &n, const declaration &)
{
    const auto var = std::get<symbol>(n.children[0]->kind_).value_;
    checkKeyword(var);
    symbol_scope::add(var);
    return 0;
}

int
evaluator::visit(const node &n, const compound_statement&)
{
    symbol_scope locals;
    auto res = 0;
    auto &c = n.children;
    for (const auto &child : c) {
        res = this->visit(*child);
    }
    return res;
}

int
evaluator::visit(const node &n, const if_statement &)
{
    const auto &cond = *n.children[0];
    auto res = visit(cond);
    if (res != 0) {
        visit(*n.children[1]);
    } else {
        if (n.children.size() == 3) {
            visit(*n.children[2]);
        }
    }
    return 0;
}

int
evaluator::visit(const node &n, const assignment_statement &)
{
    auto res = visit(*n.children[1]);
    auto var = std::get<symbol>(n.children[0]->kind_).value_;
    checkKeyword(var);
    symbol_scope::lookup(var) = res;
    std::cerr << "Result: " << var << " = " << res << std::endl;
    return res;
}

int
evaluator::visit(const node &n, const expression_statement &)
{
    auto res = visit(*n.children[0]);
    std::cerr << "Result: " << res << std::endl;
    return res;
}

int
evaluator::visit(const node &n, const symbol &sym)
{
    const auto &var = sym.value_;
    checkKeyword(var);
    return symbol_scope::lookup(var);
}

int
evaluator::visit(const node &, const number &i)
{
    return i.value_;
}

int
evaluator::visit(const node &n, const unary_minus &)
{
    return -1 * visit(*n.children[0]);
}

int
evaluator::visit(const node &n, const unary_plus &)
{
    return visit(*n.children[0]);
}

int
evaluator::visit(const node &n, const multiplication &)
{
    return visit(*n.children[0]) * visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const division &)
{
    return visit(*n.children[0]) / visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const modulus &)
{
    return visit(*n.children[0]) % visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const addition &)
{
    return visit(*n.children[0]) + visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const subtraction &)
{
    return visit(*n.children[0]) - visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const logical_or &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if ((lhs != 0) || (rhs != 0)) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const logical_or_else &)
{
    auto lhs = visit(*n.children[0]);
    if (lhs != 0) {
        return 1;
    }
    auto rhs = visit(*n.children[1]);
    if (rhs != 0) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const logical_and &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if ((lhs != 0) && (rhs != 0)) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const logical_and_then &)
{
    auto lhs = visit(*n.children[0]);
    if (lhs == 0) {
        return 0;
    }
    auto rhs = visit(*n.children[1]);
    if (rhs != 0) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const equal_to &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs == rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const not_equal &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs != rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const less_than &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs < rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const less_or_equal &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs <= rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const greater_than &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs > rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const greater_or_equal &)
{
    auto lhs = visit(*n.children[0]);
    auto rhs = visit(*n.children[1]);
    if (lhs >= rhs) {
        return 1;
    } else {
        return 0;
    }
}

int
evaluator::visit(const node &n, const function_call &f)
{
    auto operand = visit(*n.children[1]);
    auto func = std::get<symbol>(n.children[0]->kind_).value_;
    if (func == "abs") {
        return abs(operand);
    } else if (func == "sgn") {
        if (operand < 0) {
            return -1;
        } else if (operand > 0) {
            return 1;
        }
        return 0;
    }
    std::cerr << "Unknown function: " << func << " -- ignored.\n";
    return operand;
}

} // namespace Calc
