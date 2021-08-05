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


#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

#include "node.h"

namespace Calc {
class traversal;

/// Visit the parse tree.
class node_visitor
{
public:
    node_visitor() = default;
    explicit node_visitor(traversal &trav) : traversal_(&trav) { }

    node_visitor(const node_visitor &) = delete;
    node_visitor(node_visitor &&) = default;
    ~node_visitor() = default;

    node_visitor& operator=(const node_visitor &) = delete;
    node_visitor& operator=(node_visitor &&) = default;

    /// Visit a node.
    void accept(Node::node&);

    /// Visit a monostate node
    virtual void visit(Node::node &, std::monostate &);

#define xx(a, b) virtual void visit(Node::node &, Node::a &);
#include "node_kind.def"

    void set_traversal(traversal &trav)     { traversal_ = &trav; }

protected:
    traversal   *traversal_ = nullptr;

};

} // namespace Calc

#endif // VISITOR_H_INCLUDED
