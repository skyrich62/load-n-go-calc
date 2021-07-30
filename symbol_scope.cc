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

#include "symbol_scope.h"

namespace Calc {


symbol_scope* symbol_scope::current_ = nullptr;

symbol_scope::symbol_scope(Node::parent &p) : parent_(p)
{
    previous_ = current_;
    current_ = this;
    scope_ = std::make_unique<Node::node>();
    scope_ -> set_type<Node::scope>();
    scope_ -> set_kind(Node::scope{ });
}

symbol_scope::~symbol_scope()
{
    current_ = previous_;
}

Node::node*
symbol_scope::lookup(const std::string &n)
{
    auto frame = current_;
    while (frame) {
        try {
            return frame->table_.at(n);
        } catch (const std::out_of_range &) {
            frame = frame->previous_;
        }
    };
    // If we got here, we can't find the symbol. Insert a new symbol in the
    // current frame and return that value.
    // This is an implicit declaration.
    // @todo fix this
    // return add(n);
    return nullptr;
}

Node::node*
symbol_scope::add(const std::string &n, Node::node &p)
{
    current_->table_[n] = &p;
    return &p;
}

} // namespace Calc
