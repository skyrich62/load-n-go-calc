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

#include <iostream>
#define SHOW std::cout << __PRETTY_FUNCTION__ << std::endl;

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

#define xx(a, b) void pre_visit(node &n, a &) override;
#include "node_kind.def"

private:
    std::ostream &os_;

    using LinkNames = std::vector<std::string>;

    void print_link(const node &from,
                    const node &to,
                    const std::string_view s,
                    const char *color = defaultColor);
    void pre_visit_operation(node &n);
    void print_links(const node &n,
                     const LinkNames &names,
                     const char *color = defaultColor);
    void print_links(const node &n,
                     std::string_view s,
                     const char *color = defaultColor);

};

class node_decorator
{
public:
    node_decorator(const node &n);
    std::string get_color() const            { return color_; }
    std::string get_style() const            { return style_; }
    std::string get_peripheries() const      { return peripheries_; }

private:
    std::string color_{"black"};
    std::string style_{"ellipse"};
    std::string peripheries_{"1"};
};

node_decorator::node_decorator(const node &n)
{
    auto &kind = n.kind_;
    if (std::holds_alternative<variable>(kind)) {
        color_ = "purple";
    } else if (std::holds_alternative<function>(kind)) {
        color_ = "red";
        style_="square";
    }
    /// @todo figure out how to handle statements easily.
}

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
    auto seq = n.children.size() > 1;
    for (auto i = 0u; i < n.children.size(); ++i) {
        std::ostringstream os;
        os << name;
        if (seq) {
            os << " #" << i + 1;
        }
        print_link(n, *n.children[i], os.str(), color);
    }
}

void
dot_visitor::print_node(node &n, const char *color)
{
    std::string style;
    std::string name;

    if (std::holds_alternative<variable>(n.kind_)) {
        auto var = std::get<variable>(n.kind_);
        name = "Var: ";
        name += var.name_;
    }
    if (std::holds_alternative<function>(n.kind_)) {
        auto func = std::get<function>(n.kind_);
        name = "Func: ";
        name += func.name_;
        style="box";
        color = "red";
    }
    os_ << "  x" << &n
        << " [color=" << color << ", fontcolor=" << color;
    if (!style.empty()) {
        os_ << ", shape=" << style;
    }
    os_ << ", label=\"";
    auto s = n.is_root() ? "ROOT" : std::string(n.type);
    TAO_PEGTL_NAMESPACE::parse_tree::internal::escape(os_, s);
    if (!name.empty()) {
        os_ << "\\n " << name;
    }
    os_ << "\" ]\n";
}

void
dot_visitor::pre_visit(node &n, root &r)
{
    print_node(n);
    print_links(n, "statement");
    if (r.scope_) {
        accept(*r.scope_);
        print_link(n, *r.scope_, "scope");
    }
}

void
dot_visitor::pre_visit(node &n, compound_statement &s)
{
    print_node(n);
    print_links(n, "statement");
    if (s.scope_) {
        accept(*s.scope_);
        print_link(n, *s.scope_, "scope");
    }
}

void
dot_visitor::pre_visit(node &n, variable_ref &r)
{
    print_node(n);
    print_link(n, *r.variable_, "variable", varColor);
}

void
dot_visitor::pre_visit(node &n, scope &s)
{
    print_node(n);
    print_links(n, "variable", varColor);
    for (auto &var : n.children) {
        accept(*var);
    }
    if (s.parent_scope_) {
      print_link(n, *s.parent_scope_, "parent");
    }
    for (auto scope : s.subscopes_) {
        print_link(n, *scope, "subscope");
    }
}

void
dot_visitor::pre_visit(node &n, declaration &d)
{
    print_node(n);
    print_link(n, *n.children[0], "variable");
}

void
dot_visitor::pre_visit(node &n, variable &)
{
    print_node(n, varColor);
}

void
dot_visitor::pre_visit(node &n, number &)
{
    print_node(n);
}

void
dot_visitor::pre_visit(node &n, assignment_statement &)
{
    print_node(n);
    LinkNames names{"variable", "expression"};
    print_links(n, names);
}

void
dot_visitor::pre_visit(node &n, expression_statement &)
{
    print_node(n);
    print_link(n, *n.children[0], "expression");
}

void
dot_visitor::pre_visit_operation(node &n)
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
dot_visitor::pre_visit(node &n, function &f)
{
    print_node(n);
}

void
dot_visitor::pre_visit(node &n, function_call &)
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
dot_visitor::pre_visit(node &n, unary_plus &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, unary_minus &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, modulus &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, logical_or &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, logical_or_else &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, logical_and &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, logical_and_then &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, greater_than &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, greater_or_equal &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, less_than &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, less_or_equal &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, not_equal &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, equal_to &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, addition &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, subtraction &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, division &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, multiplication &)
{
    pre_visit_operation(n);
}

void
dot_visitor::pre_visit(node &n, if_statement &s)
{
    print_node(n);
    LinkNames names{"condition", "then_clause", "else_clause"};
    print_links(n, names);
}

void
print_dot(std::ostream &os, node &n)
{
    dot_visitor d(os);
    traversal trav(d, node_visitor::PRE_VISIT);
    os << "digraph parse_tree\n{\n";
    trav.traverse(n);
    os << "}\n";
}

} // namespace Calc
