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

#include <CompuBrite/CheckPoint.h>

namespace Calc {

namespace cbi = CompuBrite;

symbol_scope* symbol_scope::current_ = nullptr;

symbol_scope::symbol_scope(Node::node &n, Node::parent &p) :
    parent_node_(n),
    parent_(p)
{
    cbi::CheckPoint cp("symbol_scope");
    previous_ = current_;
    current_ = this;

    cp.print(CBI_HERE, "previous_ = ", previous_, ", current_ = ", current_, '\n');
    scope_ = std::make_unique<Node::node>();
    scope_ -> set_type<Node::scope>();

    Node::scope s;
    scope_ -> set_kind(std::move(s));

}

symbol_scope::~symbol_scope()
{
    cbi::CheckPoint cp("symbol_scope");
    cp.print(CBI_HERE, "previous_ = ", previous_, ", current_ = ", current_, '\n');
    current_ = previous_;
    if (previous_) {
        auto s = scope().get_kind<Node::scope>();
        s->parent_scope_ = previous_->scope_.get();
        auto par = s->parent_scope_->get_kind<Node::scope>();
        if (!scope_->children.empty()) {
            par->subscopes_.emplace_back(scope_.get());
        }
    }
    if (!scope_->children.empty()) {
        parent_.scope_ = std::move(scope_);
    }
}

Node::node*
symbol_scope::lookup(const std::string &name)
{
    cbi::CheckPoint cp("lookup");
    for (auto frame = current_; frame != nullptr; frame = frame->previous_) {
        auto found = frame->table_.find(name);
        if (found == frame->table_.end()) {
            cp.print(CBI_HERE, "Frame = ", frame);
            continue;
        }
        cp.print(CBI_HERE, "====> Frame: ", frame, ", Found: ", found->first, ": ", found->second);
        return found->second;
    }
    // If we got here, we can't find the symbol. Insert a new symbol in the
    // current frame, add it to the current scope, and return that value.
    // This is an implicit declaration.
    cp.print(CBI_HERE, "Inserting a new node for ", name);
    auto node = std::make_unique<Node::node>();
    auto ptr = node.get();
    node->set_kind(Node::variable{name});
    node->set_type<Node::variable>();
    current_->table_[name] = node.get();
    current_->scope_->children.emplace_back(std::move(node));
    return ptr;
}

void
symbol_scope::add(const std::string &name, Node::node &var)
{
    cbi::CheckPoint cp("add");
    auto node = std::make_unique<Node::node>();
    node->m_begin = var.m_begin;
    node->m_end   = var.m_end;
    auto n = name;    /// must make a copy.
    node->set_kind(Node::variable{n});
    node->set_type<Node::variable>();

    var.set_kind(Node::variable_ref{ node.get() });
    var.set_type<Node::variable_ref>( );
    var.remove_content();
    cp.print(CBI_HERE, "Adding ", n, ": ", node.get(), ", to frame: ", current_);
    current_->table_[n] = node.get();
    current_->scope_->children.emplace_back(std::move(node));
}

void
symbol_scope::add_intrinsic(std::function<int(int)> func,
              const std::string &name)
{
    auto node = std::make_unique<Node::node>();
    node->set_type<Node::function>();
    Node::function f;
    f.name_ = name;
    f.intrinsic_ = func;
    node->set_kind(std::move(f));
    current_->table_[name] = node.get();
    current_->scope_->children.emplace_back(std::move(node));
}

} // namespace Calc
