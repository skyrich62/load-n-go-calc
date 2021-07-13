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
        res = this->accept(*child);
    }
    return res;
}

int
evaluator::visit(const node &n, const if_statement &)
{
    const auto &cond = *n.children[0];
    auto res = accept(cond);
    if (res != 0) {
        accept(*n.children[1]);
    } else {
        if (n.children.size() == 3) {
            accept(*n.children[2]);
        }
    }
    return 0;
}

int
evaluator::visit(const node &n, const assignment_statement &)
{
    auto res = accept(*n.children[1]);
    auto var = std::get<symbol>(n.children[0]->kind_).value_;
    checkKeyword(var);
    symbol_scope::lookup(var) = res;
    std::cerr << "Result: " << var << " = " << res << std::endl;
    return res;
}

int
evaluator::visit(const node &n, const expression_statement &)
{
    auto res = accept(*n.children[0]);
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
    return -1 * accept(*n.children[0]);
}

int
evaluator::visit(const node &n, const unary_plus &)
{
    return accept(*n.children[0]);
}

int
evaluator::visit(const node &n, const multiplication &)
{
    return accept(*n.children[0]) * accept(*n.children[1]);
}

int
evaluator::visit(const node &n, const division &)
{
    return accept(*n.children[0]) / accept(*n.children[1]);
}

int
evaluator::visit(const node &n, const modulus &)
{
    return accept(*n.children[0]) % accept(*n.children[1]);
}

int
evaluator::visit(const node &n, const addition &)
{
    return accept(*n.children[0]) + accept(*n.children[1]);
}

int
evaluator::visit(const node &n, const subtraction &)
{
    return accept(*n.children[0]) - accept(*n.children[1]);
}

int
evaluator::visit(const node &n, const logical_or &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if ((lhs != 0) || (rhs != 0)) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const logical_or_else &)
{
    auto lhs = accept(*n.children[0]);
    if (lhs != 0) {
        return 1;
    }
    auto rhs = accept(*n.children[1]);
    if (rhs != 0) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const logical_and &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if ((lhs != 0) && (rhs != 0)) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const logical_and_then &)
{
    auto lhs = accept(*n.children[0]);
    if (lhs == 0) {
        return 0;
    }
    auto rhs = accept(*n.children[1]);
    if (rhs != 0) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const equal_to &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs == rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const not_equal &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs != rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const less_than &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs < rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const less_or_equal &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs <= rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const greater_than &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs > rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const greater_or_equal &)
{
    auto lhs = accept(*n.children[0]);
    auto rhs = accept(*n.children[1]);
    if (lhs >= rhs) {
        return 1;
    }
    return 0;
}

int
evaluator::visit(const node &n, const function_call &f)
{
    auto operand = accept(*n.children[1]);
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
