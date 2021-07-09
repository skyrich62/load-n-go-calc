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


#ifndef SYMBOL_SCOPE_H_INCLUDED
#define SYMBOL_SCOPE_H_INCLUDED

#include <map>

namespace Calc {

class symbol_scope
{
public:
    using Symbols = std::map<std::string, int>;

    /// Create a new stack frame and push it onto the stack.
    symbol_scope();

    /// Destroy a stack frame, pop it off the stack.
    ~symbol_scope();

    /// lookup a symbol name
    /// @return a reference to the symbol, creating it in the current
    /// stack frame if necessary.
    static int& lookup(const std::string &name);

    /// Add a new symbol to the current scope.
    /// @param name the symbol to add.
    static void add(const std::string &name);
private:
    Symbols table_;
    symbol_scope *previous_ = nullptr;
    static symbol_scope *current_;
};

} // namespace Calc


#endif // SYMBOL_SCOPE_H_INCLUDED