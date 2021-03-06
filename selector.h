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

#ifndef SELECTOR_H_INCLUDED
#define SELECTOR_H_INCLUDED

#include "node.h"
#include <CompuBrite/CheckPoint.h>

namespace Calc::grammar {
using Ptr = std::unique_ptr<Node::node>;
namespace cbi = CompuBrite;

/// Rearrange expressions to remove "factor", "term", and expression
/// nodes, and put the tree in prefix form.
struct rearrange_expr : parse_tree::apply<rearrange_expr>
{
    template <typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        cbi::CheckPoint cp("rearrange_expr");
        if (cp.active()) {
            return;
        }
        if (n->children.size() == 1) {
            n = std::move(n->children.back());
        } else if (n->children.size() == 2) {
            // this is -X
            n->remove_content();
            auto &c = n->children;
            auto rhs = std::move( c.back() );
            c.pop_back();
            auto op = std::move( c.back() );
            c.pop_back();
            op->children.emplace_back(std::move(rhs));
            n = std::move(op);
            //transform( n->children.front(), st...);
        } else if (n->children.size() > 2){
            n->remove_content();
            auto& c = n->children;
            auto r = std::move( c.back() );
            c.pop_back();
            auto o = std::move( c.back() );
            c.pop_back();
            o->children.emplace_back(std::move(n));
            o->children.emplace_back(std::move(r));
            n = std::move(o);
            transform( n->children.front(), st...);
        }
    }
};

/// Store a literal number, (integer), in the parse tree.
struct store_number : parse_tree::apply<store_number>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        std::istringstream is(n->string());
        Node::number val;
        is >> val.value_;
        n->kind_ = val;
        n->set_type<Node::number>();
    }
};

/// Store a symbol in the parse tree.
struct store_symbol : parse_tree::apply<store_symbol>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        Node::variable val;
        val.name_ = n->string();
        n->kind_ = val;
        n->set_type<Node::variable>();
    }
};

/// Attempt to set the std::variant according to the type provided.
template<typename Rule, typename Kind>
bool try_type(Ptr &n)
{
    if (n->is_type<Rule>()) {
        n->kind_ = Kind();
        n->set_type<Kind>();
        return true;
    }
    return false;
}


/// Remove the content from the node, and attempt to classify its
/// type.
struct assign_node_type : parse_tree::apply<assign_node_type>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        // statements
        try_type<decl_statement, Node::declaration>(n)                ||
        try_type<function_definition, Node::function>(n)              ||
        try_type<expression_statement, Node::expression_statement>(n) ||
        try_type<exit_statement, Node::exit_statement>(n)             ||
        try_type<return_statement, Node::return_statement>(n)         ||
        try_type<if_statement, Node::if_statement>(n)                 ||
        try_type<assignment_statement, Node::assignment_statement>(n) ||
        try_type<compound_statement, Node::compound_statement>(n)     ||

        // operations
        try_type<logical_negation, Node::logical_not>(n)              ||
        try_type<OR, Node::logical_or>(n)                             ||
        try_type<AND, Node::logical_and>(n)                           ||
        try_type<OR_ELSE, Node::logical_or_else>(n)                   ||
        try_type<AND_THEN, Node::logical_and_then>(n)                 ||
        try_type<equal_to, Node::equal_to>(n)                         ||
        try_type<not_equal, Node::not_equal>(n)                       ||
        try_type<greater_than, Node::greater_than>(n)                 ||
        try_type<greater_or_equal, Node::greater_or_equal>(n)         ||
        try_type<less_than, Node::less_than>(n)                       ||
        try_type<less_or_equal, Node::less_or_equal>(n)               ||
        try_type<function_call, Node::function_call>(n)               ||
        try_type<addition, Node::addition>(n)                         ||
        try_type<subtraction,  Node::subtraction>(n)                  ||
        try_type<multiplication, Node::multiplication>(n)             ||
        try_type<modulus, Node::modulus>(n)                           ||
        try_type<division, Node::division>(n)                         ||
        try_type<unary_plus, Node::unary_plus>(n)                     ||
        try_type<unary_minus, Node::unary_minus>(n);
        n->remove_content();
    }
};

using child_iterator = std::vector<Ptr>::iterator;

static void
rewrite_while(Ptr &n, child_iterator &w, child_iterator &cond)
{
    std::swap(*w, *cond);
    n->children.erase(cond);
}

static void
rewrite_until(Ptr &n, child_iterator &u, child_iterator &cond)
{
    // change the until to a NOT.
    try_type<UNTIL, Node::logical_not>(*u);
    // Make the condition the operand of the not.
    (*u)->emplace_back(std::move(*cond));
    // erase the now empty condition node.
    n->children.erase(cond);
}

/// Rewrite loop statements
struct rewrite_bottom_test_loop:
    parse_tree::apply<rewrite_bottom_test_loop>
{
    template <typename ... States >
    static void transform( Ptr &n, States&&... st)
    {
        try_type<bottom_test, Node::loop_bottom_test_statement>(n);
        n->remove_content();
        auto while_until = n->children.begin() + 1;
        auto cond = while_until + 1;
        if ((*while_until)->is_type<Calc::grammar::WHILE>()) {
            rewrite_while(n, while_until, cond);
        } else {
            rewrite_until(n, while_until, cond);
        }
    }
};

struct rewrite_top_test_loop:
    parse_tree::apply<rewrite_top_test_loop>
{
    template <typename ... States >
    static void transform( Ptr &n, States&&... st)
    {
        try_type<top_test, Node::loop_top_test_statement>(n);
        n->remove_content();
        auto while_until = n->children.begin();
        auto cond = while_until + 1;
        if ((*while_until)->is_type<Calc::grammar::WHILE>()) {
            rewrite_while(n, while_until, cond);
        } else {
            rewrite_until(n, while_until, cond);
        }
    }
};

template <typename Kind>
void handle_compound_and_exit_node_name(Ptr &n)
{
    n->set_type<Kind>();
    n->set_kind(Kind{});
    n->remove_content();
    if (n->children.size() == 0) {
        return;
    }
    auto &child = n->children[0];
    if (child->is_type<block_id>()) {
        n->get_kind<Kind>()->set_name(child->string());
        n->children.erase(n->children.begin());
    }
}

struct handle_compound_statement :
    parse_tree::apply< handle_compound_statement >
{
    template <typename ... States >
    static void transform( Ptr &n, States&&... st)
    {
        handle_compound_and_exit_node_name<Node::compound_statement>(n);
    }
};

struct handle_exit_statement :
    parse_tree::apply< handle_exit_statement >
{
    template <typename ... States >
    static void transform( Ptr &n, States&&... st)
    {
        handle_compound_and_exit_node_name<Node::exit_statement>(n);
    }
};

/// Used to select which nodes are created.
template <typename Rule>
using selector = parse_tree::selector<
  Rule,
  /// Uncomment to see parenthesized expression in the parse tree.
  /// Use this to track where parenthesis were in the source code in
  /// expressions.
  // parse_tree::store_content::on< parenthesized_expr >,

  /// For integers, create a number parse node.
  store_number::on< integer >,

  /// for symbol_name, create a symbol node.
  store_symbol::on< symbol_name >,

  /// for block_id, save the name.
  parse_tree::store_content::on< block_id >,

  /// Special handling for compound statements
  handle_compound_statement::on< compound_statement >,

  /// Special handling for exit statements
  handle_exit_statement::on< exit_statement >,

  /// Remove the content and classify the nodes for these.
  assign_node_type::on<
    function_definition,
    decl_statement,
    if_statement,
    assignment_statement,
    return_statement,
    expression_statement,
    addition,
    subtraction,
    multiplication,
    division,
    equal_to,
    not_equal,
    less_than,
    less_or_equal,
    greater_than,
    greater_or_equal,
    AND,
    OR,
    AND_THEN,
    OR_ELSE,
    logical_negation,
    modulus,
    unary_plus,
    unary_minus,
    function_call
  >,

  parse_tree::remove_content::on<
    WHILE,
    UNTIL
  >,

  /// Rewrite loop statements
  rewrite_bottom_test_loop::on<
    bottom_test
  >,

  rewrite_top_test_loop::on<
    top_test
  >,

  /// Rearrange the expression sub-tree nodes.
  rearrange_expr::on<
    factor,
    expression,
    term
  >
>;

} // namespace Calc::grammar
#endif // SELECTOR_H_INCLUDED
