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

#include "visitor.h"

namespace Calc {

using namespace Calc::Node;

void
node_visitor::accept(node &n)
{
    if (n.is_root()) {
        for (const auto &child : n.children) {
            accept(*child);
        }
    } else {
        accept(n, n.kind_);
    }
}

void
node_visitor::accept(node &n, node_kind &kind)
{
    std::visit([this, &n](auto &arg) { return this->visit(n, arg); }, kind);
}

void
node_visitor::visit(node &n, const std::monostate &)
{
}

void
node_visitor::visit(node &n, const declaration &)
{
}

void
node_visitor::visit(node &n, const compound_statement&)
{
}

void
node_visitor::visit(node &n, const if_statement &)
{
}

void
node_visitor::visit(node &n, const assignment_statement &)
{
}

void
node_visitor::visit(node &n, const expression_statement &)
{
}

void
node_visitor::visit(node &n, const symbol &sym)
{
}

void
node_visitor::visit(node &, const number &i)
{
}

void
node_visitor::visit(node &n, const unary_minus &)
{
}

void
node_visitor::visit(node &n, const unary_plus &)
{
}

void
node_visitor::visit(node &n, const multiplication &)
{
}

void
node_visitor::visit(node &n, const division &)
{
}

void
node_visitor::visit(node &n, const modulus &)
{
}

void
node_visitor::visit(node &n, const addition &)
{
}

void
node_visitor::visit(node &n, const subtraction &)
{
}

void
node_visitor::visit(node &n, const logical_or &)
{
}

void
node_visitor::visit(node &n, const logical_or_else &)
{
}

void
node_visitor::visit(node &n, const logical_and &)
{
}

void
node_visitor::visit(node &n, const logical_and_then &)
{
}

void
node_visitor::visit(node &n, const equal_to &)
{
}

void
node_visitor::visit(node &n, const not_equal &)
{
}

void
node_visitor::visit(node &n, const less_than &)
{
}

void
node_visitor::visit(node &n, const less_or_equal &)
{
}

void
node_visitor::visit(node &n, const greater_than &)
{
}

void
node_visitor::visit(node &n, const greater_or_equal &)
{
}

void
node_visitor::visit(node &n, const function_call &f)
{
}

} // namespace Calc
