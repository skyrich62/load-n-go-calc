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


#ifndef EVALUATOR_H_INCLUDED
#define EVALUATOR_H_INCLUDED

#include "node.h"
#include "symbol_scope.h"
#include "visitor.h"

namespace Calc {
/// Evaluate the parse tree.
/// Once the parse has completed, traverse the parse tree evaluating the nodes to
/// produce a result.
class evaluator : public node_visitor
{
public:
    evaluator() = default;
    evaluator(const evaluator &) = delete;
    evaluator(evaluator &&) = default;
    ~evaluator() = default;

    evaluator& operator=(const evaluator &) = delete;
    evaluator& operator=(evaluator &&) = default;

    /// Visit a declaration
    int visit(const Node::node &, const Node::declaration &) override;

    /// Visit an if statement.  Evaluate the expression, if the result it non-zero,
    /// evaluate the children.
    int visit(const Node::node &, const Node::if_statement &) override;

    /// Visit an assignment statement.  Evaluate the RHS, and assign the result
    /// to the given symbol_name, (LHS).
    int visit(const Node::node &, const Node::assignment_statement &) override;

    /// Visit an expression statement.  Evaluate the expression and print the result.
    int visit(const Node::node &, const Node::expression_statement &) override;

    /// Visit a compound statement.  Evaluate each statement.
    int visit(const Node::node &, const Node::compound_statement &) override;

    /// Visit an addition node. Evalate the LHS, evaluate the RHS, add them together,
    /// and return the result.
    int visit(const Node::node &, const Node::addition &) override;

    /// Visit a subtraction node. Evaluate the LHS and RHS, subtract the
    /// RHS from the LHS, return the result.
    int visit(const Node::node &, const Node::subtraction &) override;

    /// Visit a multiplication node. Evalute the LHS and RHS, multiply them, return
    /// the result.
    int visit(const Node::node &, const Node::multiplication &) override;

    /// Visit a division node.  Evaluate the LHS and RHS, divide LHS by RHS.
    int visit(const Node::node &, const Node::division &) override;

    /// Visit a modulus node.  Evaluate the LHS % RHS
    int visit(const Node::node &, const Node::modulus &) override;


    /// Visit an equal_to node.  Evaluate LHS and RHS if they are equal,
    /// return 1, else 0.
    int visit(const Node::node &, const Node::equal_to &) override;

    /// Visit a not_equal_node.  Evaluate LHS and RHS if they are not equal,
    /// return 1, else 0.
    int visit(const Node::node &, const Node::not_equal &) override;

    /// Visit a greater_than node.  Evaluate LHS, and RHS, if LHS > RHS,
    /// return 1, else 0.
    int visit(const Node::node &, const Node::greater_than &) override;

    /// Visit a greater_or_equal node.  Evaluate LHS and RHS, if LHS >= RHS
    /// return 1, else 0.
    int visit(const Node::node &, const Node::greater_or_equal &) override;

    /// Visit a less_than node.  Evaluate LHS and RHS, if LHS < RHS return
    /// 1 else 0.
    int visit(const Node::node &, const Node::less_than &) override;

    /// Visit a less_or_equal node.  Evaluate LHS and RHS, if LHS <= RHS
    /// return 1, else 0.
    int visit(const Node::node &, const Node::less_or_equal &) override;

    /// Visit an logical_and node.  Evaluate LHS and RHS.
    /// Return 1 if both are non-zero else 0.
    int visit(const Node::node &, const Node::logical_and &) override;

    /// Visit an logical_and_then node.
    /// Evaluate the LHS if it is non-zero evaluate the RHS, if that is also
    /// non-zero return 1 else 0.
    int visit(const Node::node &, const Node::logical_and_then &) override;

    /// Visit an logical_or node.
    /// Evaluate the LHS and the RHS, return 1 if either is non-zero, else 0.
    int visit(const Node::node &, const Node::logical_or &) override;

    /// Visit an logical_or_else node.
    /// Visit the LHS, if it is non-zero, return 1.
    /// Otherwise, visit the RHS and return 1 if it's non-zero, else return 0.
    int visit(const Node::node &, const Node::logical_or_else &) override;

    /// Visit a unary_minus node. Evaluate the operand, negate it and return the result.
    int visit(const Node::node &, const Node::unary_minus &) override;

    /// Visit a unary_plus node.  Evaluate the operand, and return it.
    int visit(const Node::node &, const Node::unary_plus &) override;

    /// Visit a symbol, (symbol_name), node.  Store the symbol name in the node,
    /// Return the value of the symbol from the symbol table.  If the symbol has
    /// never before been seen, set it's value to zero.
    int visit(const Node::node &, const Node::symbol &) override;

    /// Visit a number, (integer), node.  Store the binary value of the integer in
    /// the node.
    int visit(const Node::node &, const Node::number &) override;

    /// Visit a function_call node.  If the function is not known, return 0.
    /// Otherwise, evaluate the operand expression, apply the function and return
    /// the value.
    /// @note
    /// Currently, the only known functions are:
    ///  "abs" for absolute value.
    ///  "sgn" returns -1, 0, or 1, depending on the sign of the operand.
    int visit(const Node::node &, const Node::function_call &) override;

private:
    symbol_scope globals;

};
} // namespace Calc


#endif // EVALUATOR_H_INCLUDED
