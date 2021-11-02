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

#include "semantic_analysis.h"
#include <iostream>
#include <set>

#include <CompuBrite/CheckPoint.h>

namespace Calc {
namespace cbi = CompuBrite;

using namespace Calc::Node;

void
checkKeyword(const std::string &name)
{
    static std::set<std::string> keywords{
        "if", "else", "and", "then", "or", "and", "var", "def", "exit",
        "while", "until", "loop"
    };

    if (auto found = keywords.find(name); found != keywords.end())  {
        std::cerr << "Warning: keyword '" << name << "' used as symbol_name."
                  << std::endl;
    }
}

semantic_analysis::semantic_analysis(Node::node &n, Node::parent &p)
{
    push_scope(n, p);
    add_intrinsics();
}

semantic_analysis::~semantic_analysis()
{
    pop_scope();
}

void
semantic_analysis::add_intrinsics()
{
    symbol_scope::add_intrinsic(
        [](int x)
        {
            if (x < 0) {
                return -x;
            }
            return x;
        }, "abs");
    symbol_scope::add_intrinsic(
        [](int x)
        {
            if (x < 0) {
                return -1;
            }
            if (x > 0) {
                return 1;
            }
            return 0;
        }, "sgn");
}

void
semantic_analysis::push_scope(Node::node &node, Node::parent &parent)
{
    auto ptr = std::make_unique<symbol_scope>(node, parent);
    cbi::CheckPoint cp("semantic_analysis");
    cp.print(CBI_HERE, "New scope: ", ptr.get());
    stack_.emplace(std::move(ptr));
}

void
semantic_analysis::pop_scope()
{
    cbi::CheckPoint cp("semantic_analysis");
    if (stack_.empty()) {
        cp.hit(CBI_HERE, "Stack is empty, shouldn't have happened!");
        return;
    }
    auto &ptr = stack_.top();
    cp.print(CBI_HERE, "Ptr = ", ptr.get());
    stack_.pop();
}

void
semantic_analysis::pre_visit(node &n, loop_top_test_statement &)
{
    ++loops_;
}

void
semantic_analysis::pre_visit(node &n, loop_bottom_test_statement &)
{
    ++loops_;
}

void
semantic_analysis::post_visit(node &n, loop_top_test_statement &)
{
    --loops_;
}

void
semantic_analysis::post_visit(node &n, loop_bottom_test_statement &)
{
    --loops_;
}

void
semantic_analysis::pre_visit(node &n, exit_statement &)
{
    if (loops_ == 0u) {
        /// @todo Write error handler that will report position of the error.
        std::cerr << "Error: Exit statement is only allowed inside loop bodies."
                  << std::endl;
        n.set_type<error>();
        n.set_kind(error{ });
    }
}

void
semantic_analysis::pre_visit(node &n, return_statement &)
{
    if (funcs_ == 0u) {
        /// @todo Write error handler that will report position of the error.
        std::cerr << "Error: Return statement is only allowed inside function bodies." << std::endl;
        n.set_type<error>();
        n.set_kind(error{ });
        n.children.clear();
    }
}

void
semantic_analysis::pre_visit(node &n, function_call &fc)
{
    cbi::CheckPoint cp("sa-function-call");
    cp.print(CBI_HERE);
    // First, capture the function name node, then delete it from
    // the children.  Find the function associated with the name,
    // and put it in the function_call node.
    auto fnode = std::move(n.children[0]);
    n.children.erase(n.children.begin());
    auto &name = fnode->get_kind<variable>()->name_;
    checkKeyword(name);
    auto r = symbol_scope::lookup(name);
    fc.symbol_ = r;
}

void
semantic_analysis::pre_visit(node &n, declaration &)
{
    traversal_->disableSubTree();
    auto &child = n.children[0];
    auto &var = child->get_kind<variable>()->name_;
    checkKeyword(var);
    symbol_scope::add(var, *child);
}

void
semantic_analysis::pre_visit(node &n, compound_statement &c)
{
    push_scope(n, c);
    auto &s = stack_.top();
    s->name(c.name_);
}

void
semantic_analysis::post_visit(node &, compound_statement &c)
{
    pop_scope();
}

void
semantic_analysis::pre_visit(node &n, function &f)
{
    ++funcs_;
    // First put the name of the function in the function node.
    auto iter = n.children.begin();
    f.name_ = (*iter)->string();

    // Add the function to the current scope
    symbol_scope::add_function(f.name_, n);

    // Push a new scope for this function.
    push_scope(n, f);
    symbol_scope::current()->scope().get_kind<scope>()->function_ = 1;

    for (++iter; iter != n.children.end(); ++iter) {
        // Now, put the parameters into the function's scope.
        auto &child = *iter;
        if (!child->is_type<variable>()) {
            break;
        }
        auto &var = child->get_kind<variable>()->name_;
        symbol_scope::add(var, *child);
    }
    n.children.erase(n.children.begin(), iter);
}

void
semantic_analysis::post_visit(node &n, function &f)
{
    --funcs_;
    pop_scope();
    // Now, unlink this function from it's parent, and link it to
    // the current scope.
#if 0
    auto &parent_node = symbol_scope::current()->parent_node();
    auto &scope = symbol_scope::current()->scope();
    auto iter = parent_node.children.begin();
    for (; iter != parent_node.children.end(); ++iter) {
        auto &child = *iter;
        if (child.get() == &n) {
            auto &parent = symbol_scope::current()->parent();
            scope.emplace_back(std::move(child));
            parent_node.children.erase(iter);
            break;
        }
    }
#endif
}

void
semantic_analysis::pre_visit(node &n, variable &)
{
    auto &name = n.get_kind<variable>()->name_;
    checkKeyword(name);
    auto r = symbol_scope::lookup(name);
    n.set_kind(variable_ref{r});
    n.set_type<variable_ref>();
    n.remove_content();
}

} // namespace Calc
