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

#include <iostream>

namespace Calc {

using namespace Calc::Node;

void
node_visitor::accept(node &n, Mode mode)
{
    auto &kind = n.kind_;
    std::visit([this, &n, mode](auto &arg)
        {
            if (mode == PRE_VISIT) {
                return this->pre_visit(n, arg);
            } else {
                return this->post_visit(n, arg);
            }
        }, kind);
}

void
node_visitor::pre_visit(node &, std::monostate &)
{
}

void
node_visitor::post_visit(node &, std::monostate &)
{
}

#define xx(a, b)  void node_visitor::pre_visit(node &n, a &) {  }
#include "node_kind.def"
#define xx(a, b)  void node_visitor::post_visit(node &n, a &) {  }
#include "node_kind.def"

} // namespace Calc
