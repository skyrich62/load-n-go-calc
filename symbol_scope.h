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


#ifndef SYMBOL_SCOPE_H_INCLUDED
#define SYMBOL_SCOPE_H_INCLUDED

#include <map>
#include "node.h"

namespace Calc {

class symbol_scope
{
public:
    using Symbols = std::map<std::string, Node::node *>;

    /// Create a new stack frame and push it onto the stack.
    symbol_scope(Node::parent &p);

    /// Destroy a stack frame, pop it off the stack.
    ~symbol_scope();

    /// Set the name of the current scope
    void name(const std::string &n)         { name_ = n; }

    /// lookup a symbol name
    /// @return a reference to the symbol, creating it in the current
    /// stack frame if necessary.
    static Node::node* lookup(const std::string &name);

    /// Add a new symbol to the current scope.
    /// @param name the symbol to add.
    /// @param var the node which declares the variable.
    /// Creates a new variable node, and adds it to the current scope.
    /// Changes var to be a variable reference instead.
    static void add(std::string name, Node::node &var);
private:

    /// The symbol table for this scope.
    Symbols             table_;

    /// The name of this scope, (i.e. for named compound statements).
    std::string         name_;

    /// A pointer to the current scope node, used to add symbols.
    Node::Ptr           scope_;

    /// A pointer to the previous symbol_scope or nullptr if at root.
    symbol_scope        *previous_ = nullptr;

    /// A reference to the parent node for this scope, (root or compound stmt).
    Node::parent        &parent_;

    /// A pointer to the current symbol_scope.
    static symbol_scope *current_;
};

} // namespace Calc


#endif // SYMBOL_SCOPE_H_INCLUDED
