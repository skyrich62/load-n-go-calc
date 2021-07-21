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

#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include "dotter.h"
#include "visitor.h"
#include "traversal.h"

namespace Calc {

using namespace Calc::Node;

class dot_visitor : public node_visitor
{
public:
    dot_visitor(std::ostream &os) : os_(os) { }
    dot_visitor(const dot_visitor &) = delete;
    dot_visitor& operator=(const dot_visitor &) = delete;

    void visit(node &n, const variable_ref &) override;
    void visit(node &n, const declaration &) override;
    void visit(node &n, const variable &) override;
    void print_node(node &n);

    void print_link(const node &from, const node &to, const std::string_view s);

private:
    std::ostream &os_;
};

void
dot_visitor::print_link(const node &from, const node &to, const std::string_view s)
{
    os_ << "  x" << &from << " -> x" << &to
        << " [label=\"" << s << "\"]\n";
}

void
dot_visitor::print_node(node &n)
{
    os_ << "  x" << &n << " [label=\"";
    auto s = n.is_root() ? "ROOT" : n.type;
    TAO_PEGTL_NAMESPACE::parse_tree::internal::escape(os_, s);
    if (n.has_content()) {
        os_ << "\\n\\\"";
        TAO_PEGTL_NAMESPACE::parse_tree::internal::escape(os_, n.string_view());
        os_ << "\\\"";
    }
    os_ << "\" ]\n";
}

void
dot_visitor::visit(node &n, const variable_ref &r)
{
    print_node(n);
    print_link(n, *r.variable_, "variable");
}

void
dot_visitor::visit(node &n, const declaration &d)
{
    print_node(n);
    print_link(n, *n.children[0], "variable");
}

void
dot_visitor::visit(node &n, const variable &)
{
    print_node(n);
}

void
print_dot(std::ostream &os, node &n)
{
    dot_visitor d(os);
    traversal trav(d);
    os << "digraph parse_tree\n{\n";
    trav.traverse(n);
    os << "}\n";
}

} // namespace Calc
