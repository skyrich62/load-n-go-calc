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


#ifndef EVALUATOR_H_INCLUDED
#define EVALUATOR_H_INCLUDED

#include "node.h"
#include "visitor.h"

namespace Calc {
/// Evaluate the parse tree.
/// Once the parse has completed, traverse the parse tree evaluating the nodes to
/// produce a result.
class evaluator : public node_visitor
{
public:
    evaluator() = default;
    evaluator(const evaluator &) = delete;
    evaluator(evaluator &&) = default;
    ~evaluator() = default;

    evaluator& operator=(const evaluator &) = delete;
    evaluator& operator=(evaluator &&) = default;

#define xx(a, b) void visit(Node::node &, Node::a &) override;
#include "node_kind.def"
#undef xx
#undef yy

    /// Set the result of the current evaluation.
    void set_result(int res)                { result_ = res; }

private:
    int          result_{0};
};
} // namespace Calc


#endif // EVALUATOR_H_INCLUDED
