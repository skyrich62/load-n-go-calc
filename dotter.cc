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

const char *defaultColor = "black";
const char *varColor     = "purple";

using namespace Calc::Node;

class dot_visitor : public node_visitor
{
public:
    dot_visitor(std::ostream &os) : os_(os) { }
    dot_visitor(const dot_visitor &) = delete;
    dot_visitor& operator=(const dot_visitor &) = delete;
    void print_node(node &n, const char *color = defaultColor);

#define xx(a, b) void visit(node &n, a &) override;
#include "node_kind.def"
#undef xx

private:
    std::ostream &os_;

    using LinkNames = std::vector<std::string>;

    void print_link(const node &from,
                    const node &to,
                    const std::string_view s,
                    const char *color = defaultColor);
    void visit_operation(node &n);
    void print_links(const node &n,
                     const LinkNames &names,
                     const char *color = defaultColor);
    void print_links(const node &n,
                     std::string_view s,
                     const char *color = defaultColor);

};

void
dot_visitor::print_link(const node &from,
                        const node &to,
                        std::string_view s,
                        const char *color)
{
    os_ << "  x" << &from << " -> x" << &to;
    if (!s.empty()) {
        os_ << " [color= "<< color << ", fontcolor= " << color
            << ", label=\"" << s << "\"]";
    }
    os_ << '\n';
}

void
dot_visitor::print_links(const node &n,
                         const LinkNames &names,
                         const char *color)
{
    for (auto i = 0u; i < n.children.size(); ++i) {
        print_link(n, *n.children[i], names[i], color);
    }
}

void
dot_visitor::print_links(const node &n,
                         std::string_view name,
                         const char *color)
{
    for (auto i = 0u; i < n.children.size(); ++i) {
        print_link(n, *n.children[i], name, color);
    }
}

void
dot_visitor::print_node(node &n, const char *color)
{
    os_ << "  x" << &n
        << " [color= " << color << ", fontcolor=" << color
        << ", label=\"";
    auto s = n.is_root() ? "ROOT" : std::string(n.type);
    TAO_PEGTL_NAMESPACE::parse_tree::internal::escape(os_, s);
    if (n.has_content()) {
        os_ << "\\n\\\"";
        TAO_PEGTL_NAMESPACE::parse_tree::internal::escape(os_, n.string_view());
        os_ << "\\\"";
    }
    os_ << "\" ]\n";
}

void
dot_visitor::visit(node &n, root &r)
{
    print_node(n);
    print_links(n, "statement");
    if (r.scope_) {
        accept(*r.scope_);
        print_link(n, *r.scope_, "scope");
    }
}

void
dot_visitor::visit(node &n, compound_statement &s)
{
    print_node(n);
    print_links(n, "statement");
    if (s.scope_) {
        accept(*s.scope_);
    }
}

void
dot_visitor::visit(node &n, variable_ref &r)
{
    print_node(n);
    print_link(n, *r.variable_, "variable", varColor);
}

void
dot_visitor::visit(node &n, scope &s)
{
    print_node(n);
    print_links(n, "variable", varColor);
    for (auto &var : n.children) {
        accept(*var);
    }
}

void
dot_visitor::visit(node &n, declaration &d)
{
    print_node(n);
    print_link(n, *n.children[0], "variable");
}

void
dot_visitor::visit(node &n, variable &)
{
    print_node(n, varColor);
}

void
dot_visitor::visit(node &n, number &)
{
    print_node(n);
}

void
dot_visitor::visit(node &n, assignment_statement &)
{
    print_node(n);
    LinkNames names{"variable", "expression"};
    print_links(n, names);
}

void
dot_visitor::visit(node &n, expression_statement &)
{
    print_node(n);
    print_link(n, *n.children[0], "expression");
}

void
dot_visitor::visit_operation(node &n)
{
    print_node(n);
    if (n.children.size() == 1) {
        print_links(n, "operand");
    } else {
        const static LinkNames binaryNames{"lhs", "rhs"};
        print_links(n, binaryNames);
    }
}

void
dot_visitor::visit(node &n, function_call &)
{
    print_node(n);
    print_link(n, *n.children[0], "function");
    for (auto i = 1u; i < n.children.size(); ++i) {
        std::ostringstream os;
        os << "arg #" << i;
        print_link(n, *n.children[i], os.str());
    }
}

void
dot_visitor::visit(node &n, unary_plus &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, unary_minus &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, modulus &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, logical_or &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, logical_or_else &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, logical_and &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, logical_and_then &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, greater_than &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, greater_or_equal &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, less_than &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, less_or_equal &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, not_equal &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, equal_to &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, addition &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, subtraction &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, division &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, multiplication &)
{
    visit_operation(n);
}

void
dot_visitor::visit(node &n, if_statement &s)
{
    print_node(n);
    LinkNames names{"condition", "then_clause", "else_clause"};
    print_links(n, names);
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
