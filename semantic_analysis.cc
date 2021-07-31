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

#define SHOW // std::cerr << __PRETTY_FUNCTION__ << std::endl

namespace Calc {

using namespace Calc::Node;

void
checkKeyword(const std::string &name)
{
    static std::set<std::string> keywords{
        "if", "else", "and", "then", "or", "and", "var"
    };

    if (auto found = keywords.find(name); found != keywords.end())  {
        std::cerr << "Warning: keyword '" << name << "' used as symbol_name."
                  << std::endl;
    }
}

semantic_analysis::semantic_analysis(parent &p) :
    scope_(p)
{
}

void
semantic_analysis::visit(node &n, declaration &)
{
    SHOW;
    traversal_->disableSubTree();
    auto &child = n.children[0];
    auto &var = std::get<variable>(child->kind_).name_;
    checkKeyword(var);
    symbol_scope::add(var, *child);
}

void
semantic_analysis::visit(node &n, compound_statement &c)
{
    symbol_scope scope(c);
    SHOW;
}

void
semantic_analysis::visit(node &n, variable &)
{
    SHOW;
    auto &name = std::get<variable>(n.kind_).name_;
    checkKeyword(name);
    auto r = symbol_scope::lookup(name);
    n.set_kind(variable_ref{r});
    n.set_type<variable_ref>();
}

} // namespace Calc
