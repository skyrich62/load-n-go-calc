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
#include "symbol_scope.h"
#include "traversal.h"

#include <memory>
#include <stack>
#include <vector>

namespace Calc {
/// Evaluate the parse tree.
/// Once the parse has completed, traverse the parse tree evaluating the nodes to
/// produce a result.
class semantic_analysis : public node_visitor
{
public:
    semantic_analysis(Node::parent &parent);
    semantic_analysis(const semantic_analysis &) = delete;
    semantic_analysis(semantic_analysis &&) = default;
    ~semantic_analysis() = default;

    semantic_analysis& operator=(const semantic_analysis &) = delete;
    semantic_analysis& operator=(semantic_analysis &&) = default;

    /// Visit a declaration
    void pre_visit(Node::node &, Node::declaration &) override;

    /// Visit a compound statement.  Evaluate each statement.
    void pre_visit(Node::node &, Node::compound_statement &) override;

    /// Visit compound statement, after visiting its children
    void post_visit(Node::node &, Node::compound_statement &) override;

    /// Visit a symbol node.
    void pre_visit(Node::node &, Node::variable &) override;

    /// Push a new scope onto the symbol_scope stack.
    void push_scope(Node::parent &parent);

    /// Pop a scope off the stack, and delete it.
    void pop_scope();
private:
    using ScopePtr = std::unique_ptr<symbol_scope>;
    using ScopeStack = std::stack<ScopePtr, std::vector<ScopePtr>>;

    ScopeStack stack_;
};

} // namespace Calc


#endif // SEMANTIC_ANALYSIS_H_INCLUDED
