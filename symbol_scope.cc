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
    scope_ = std::make_unique<Node::node>();
    scope_ -> set_type<Node::scope>();

    Node::scope s;
    scope_ -> set_kind(std::move(s));

    previous_ = current_;
    current_ = this;
}

symbol_scope::~symbol_scope()
{
    if (previous_) {
        auto &s = std::get<Node::scope>(scope_->kind_);
        s.parent_scope_ = previous_->scope_.get();
        auto &par = std::get<Node::scope>(s.parent_scope_->kind_);
        par.subscopes_.emplace_back(scope_.get());
    }
    if (!scope_->children.empty()) {
        parent_.scope_ = std::move(scope_);
    }
    current_ = previous_;
}

Node::node*
symbol_scope::lookup(const std::string &name)
{
    auto frame = current_;
    while (frame) {
        try {
            return frame->table_.at(name);
        } catch (const std::out_of_range &) {
            frame = frame->previous_;
        }
    };
    // If we got here, we can't find the symbol. Insert a new symbol in the
    // current frame, add it to the current scope, and return that value.
    // This is an implicit declaration.
    auto node = std::make_unique<Node::node>();
    auto ptr = node.get();
    node->set_kind(Node::variable{name});
    node->set_type<Node::variable>();
    current_->table_[name] = node.get();
    current_->scope_->children.emplace_back(std::move(node));
    return ptr;
}

void
symbol_scope::add(std::string name, Node::node &var)
{

    auto node = std::make_unique<Node::node>();
    node->m_begin = var.m_begin;
    node->m_end   = var.m_end;
    node->set_kind(Node::variable{name});
    node->set_type<Node::variable>();

    var.set_kind(Node::variable_ref{ node.get() });
    var.set_type<Node::variable_ref>( );
    var.remove_content();
    current_->table_[name] = node.get();
    current_->scope_->children.emplace_back(std::move(node));
}

} // namespace Calc
