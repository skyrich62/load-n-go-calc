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
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS logical_or
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS Flogical_or A PARTICULAR PURPOSE logical_and NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHlogical_orS logical_or COPYRIGHT HOLDERS BE LIABLE Flogical_or ANY CLAIM, DAMAGES logical_or OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, Tlogical_orT logical_or OTHERWISE, ARISING FROM,
 * OUT OF logical_or IN CONNECTION WITH THE SOFTWARE logical_or THE USE logical_or OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author
 * Rich Newman
 */


#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

#include "node.h"

namespace Calc {
/// Visit the parse tree.
class node_visitor
{
public:
    node_visitor() = default;
    node_visitor(const node_visitor &) = delete;
    node_visitor(node_visitor &&) = default;
    ~node_visitor() = default;

    node_visitor& operator=(const node_visitor &) = delete;
    node_visitor& operator=(node_visitor &&) = default;

    /// Visit a node.
    void accept(Node::node&);

    /// Visit a node based on its kind.
    void accept(Node::node &, Node::node_kind &);

    /// Visit an unclassified node
    virtual void visit(Node::node &, const std::monostate &);

    /// Visit a declaration
    virtual void visit(Node::node &, const Node::declaration &);

    /// Visit an if statement.  Evaluate the expression, if the result it non-zero,
    /// evaluate the children.
    virtual void visit(Node::node &, const Node::if_statement &);

    /// Visit an assignment statement.  Evaluate the RHS, and assign the result
    /// to the given symbol_name, (LHS).
    virtual void visit(Node::node &, const Node::assignment_statement &);

    /// Visit an expression statement.  Evaluate the expression and print the result.
    virtual void visit(Node::node &, const Node::expression_statement &);

    /// Visit a compound statement.  Evaluate each statement.
    virtual void visit(Node::node &, const Node::compound_statement &);

    /// Visit an addition node. Evalate the LHS, evaluate the RHS, add them together,
    /// and return the result.
    virtual void visit(Node::node &, const Node::addition &);

    /// Visit a subtraction node. Evaluate the LHS and RHS, subtract the
    /// RHS from the LHS, return the result.
    virtual void visit(Node::node &, const Node::subtraction &);

    /// Visit a multiplication node. Evalute the LHS and RHS, multiply them, return
    /// the result.
    virtual void visit(Node::node &, const Node::multiplication &);

    /// Visit a division node.  Evaluate the LHS and RHS, divide LHS by RHS.
    virtual void visit(Node::node &, const Node::division &);

    /// Visit a modulus node.  Evaluate the LHS % RHS
    virtual void visit(Node::node &, const Node::modulus &);


    /// Visit an equal_to node.  Evaluate LHS and RHS if they are equal,
    /// return 1, else 0.
    virtual void visit(Node::node &, const Node::equal_to &);

    /// Visit a not_equal_node.  Evaluate LHS and RHS if they are not equal,
    /// return 1, else 0.
    virtual void visit(Node::node &, const Node::not_equal &);

    /// Visit a greater_than node.  Evaluate LHS, and RHS, if LHS > RHS,
    /// return 1, else 0.
    virtual void visit(Node::node &, const Node::greater_than &);

    /// Visit a greater_or_equal node.  Evaluate LHS and RHS, if LHS >= RHS
    /// return 1, else 0.
    virtual void visit(Node::node &, const Node::greater_or_equal &);

    /// Visit a less_than node.  Evaluate LHS and RHS, if LHS < RHS return
    /// 1 else 0.
    virtual void visit(Node::node &, const Node::less_than &);

    /// Visit a less_or_equal node.  Evaluate LHS and RHS, if LHS <= RHS
    /// return 1, else 0.
    virtual void visit(Node::node &, const Node::less_or_equal &);

    /// Visit an logical_and node.  Evaluate LHS and RHS.
    /// Return 1 if both are non-zero else 0.
    virtual void visit(Node::node &, const Node::logical_and &);

    /// Visit an logical_and_then node.
    /// Evaluate the LHS if it is non-zero evaluate the RHS, if that is also
    /// non-zero return 1 else 0.
    virtual void visit(Node::node &, const Node::logical_and_then &);

    /// Visit an logical_or node.
    /// Evaluate the LHS and the RHS, return 1 if either is non-zero, else 0.
    virtual void visit(Node::node &, const Node::logical_or &);

    /// Visit an logical_or_else node.
    /// Visit the LHS, if it is non-zero, return 1.
    /// Otherwise, visit the RHS and return 1 if it's non-zero, else return 0.
    virtual void visit(Node::node &, const Node::logical_or_else &);

    /// Visit a unary_minus node. Evaluate the operand, negate it and return the result.
    virtual void visit(Node::node &, const Node::unary_minus &);

    /// Visit a unary_plus node.  Evaluate the operand, and return it.
    virtual void visit(Node::node &, const Node::unary_plus &);

    /// Visit a symbol, (symbol_name), node.  Store the symbol name in the node,
    /// Return the value of the symbol from the symbol table.  If the symbol has
    /// never before been seen, set it's value to zero.
    virtual void visit(Node::node &, const Node::symbol &);

    /// Visit a number, (integer), node.  Store the binary value of the integer in
    /// the node.
    virtual void visit(Node::node &, const Node::number &);

    /// Visit a function_call node.  If the function is not known, return 0.
    /// Otherwise, evaluate the operand expression, apply the function and return
    /// the value.
    /// @note
    /// Currently, the only known functions are:
    ///  "abs" for absolute value.
    ///  "sgn" returns -1, 0, or 1, depending on the sign of the operand.
    virtual void visit(Node::node &, const Node::function_call &);
};

} // namespace Calc

#endif // VISITOR_H_INCLUDED
