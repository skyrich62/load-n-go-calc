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


#ifndef TRAVERSAL_H_INCLUDED
#define TRAVERSAL_H_INCLUDED

#include "node.h"

namespace Calc {
class node_visitor;

/// Visit the parse tree.
class traversal
{
public:
    traversal(node_visitor &visitor, int mode);

    traversal(const traversal &) = delete;
    traversal(traversal &&) = default;
    ~traversal() = default;

    traversal& operator=(const traversal &) = delete;
    traversal& operator=(traversal &&) = default;

    /// Traverse a tree begining at the given node.
    void traverse(Node::node&);

    /// Disable traversal of the current sub-tree.
    void disableSubTree()                   { disable_ = true; }

    /// Stop the traversal.
    void stop()                             { stop_ = true; }

    /// Reset all traversal flags.
    void reset()
    {
        stop_ = false;
        disable_ = false;
    }

private:
    node_visitor &visitor_;
    int          mode_;
    bool         disable_{false};
    bool         stop_{false};
};

} // namespace Calc

#endif // TRAVERSAL_H_INCLUDED
