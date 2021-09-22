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

semantic_analysis::semantic_analysis(Node::parent &parent)
{
    push_scope(parent);
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
semantic_analysis::push_scope(Node::parent &parent)
{
    auto ptr = std::make_unique<symbol_scope>(parent);
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
        std::cerr << "Error: Exit statement is only allowed inside loop bodies."
                  << std::endl;
    }
}

void
semantic_analysis::pre_visit(node &n, function_call &fc)
{
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
    push_scope(c);
}

void
semantic_analysis::post_visit(node &, compound_statement &c)
{
    pop_scope();
}

void
semantic_analysis::pre_visit(node &n, function &f)
{
    push_scope(f);
    symbol_scope::current()->scope().get_kind<scope>()->function_ = 1;
    auto iter = n.children.begin();
    // First put the name of the function in the function node.
    f.name_ = (*iter)->string();
    for (++iter; iter != n.children.end(); ++iter) {
        // Now, put the parameters into the function's scope.
        auto &child = *iter;
        if (child->is_type<variable>()) {
            auto &var = child->get_kind<variable>()->name_;
            symbol_scope::add(var, *child);
        } else {
            break;
        }
    }
    n.children.erase(n.children.begin(), iter);
}

void
semantic_analysis::post_visit(node &, function &)
{
    pop_scope();
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
