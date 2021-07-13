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


#ifndef SEMANTIC_ANALYSIS_H_INCLUDED
#define SEMANTIC_ANALYSIS_H_INCLUDED

#include "node.h"
#include "visitor.h"

namespace Calc {
/// Evaluate the parse tree.
/// Once the parse has completed, traverse the parse tree evaluating the nodes to
/// produce a result.
class semantic_analysis : public node_visitor
{
public:
    semantic_analysis() = default;
    semantic_analysis(const semantic_analysis &) = delete;
    semantic_analysis(semantic_analysis &&) = default;
    ~semantic_analysis() = default;

    semantic_analysis& operator=(const semantic_analysis &) = delete;
    semantic_analysis& operator=(semantic_analysis &&) = default;

    /// Visit a declaration
    void visit(Node::node &, const Node::declaration &) override;

    /// Visit a compound statement.  Evaluate each statement.
    void visit(Node::node &, const Node::compound_statement &) override;

    /// Visit a symbol node.
    void visit(Node::node &, const Node::symbol &) override;

};
} // namespace Calc


#endif // SEMANTIC_ANALYSIS_H_INCLUDED
