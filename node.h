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

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <variant>

template< typename T, typename U> struct is_valid;

template< typename T, typename... Ts>
struct is_valid<T, std::variant< Ts... > > :
std::bool_constant< (std::is_same_v< T, Ts > || ...) >
{ };

namespace Calc::Node {
struct node;

/// Define the members of the variant in the parse tree nodes.

/// A node which holds a number, (integer).  For now, this only holds the
/// value, but can be modified to hold addtional information, ( e.g. how
/// the number was parsed, "011" vs "9" for instance).
struct number
{
    int value_;
};

/// A node which holds a symbol.  Again, for now, this is only the name
/// of the symbol, but other information could be placed here.
struct variable
{
    std::string name_;
};

struct variable_ref
{
    node *variable_;
};

struct if_statement { };
struct declaration { };
struct compound_statement { };
struct assignment_statement { };
struct expression_statement { };
struct subtraction { };
struct addition { };
struct multiplication { };
struct division { };
struct equal_to { };
struct not_equal { };
struct greater_than { };
struct greater_or_equal { };
struct less_than { };
struct less_or_equal { };
struct logical_and { };
struct logical_or { };
struct logical_and_then { };
struct logical_or_else { };
struct modulus { };
struct unary_plus { };
struct unary_minus { };
struct function_call { };

/// The variant which will hold information specific to the type of parse
/// tree node.
using node_kind =
    std::variant <
        std::monostate,       ///< Not classified yet.
        declaration,          ///< declaration:          var x;
        if_statement,         ///< if statement:         if (x) stm;
        compound_statement,   ///< Compound statement:   { statement+ }
        assignment_statement, ///< Assignment statement: x:= 5;
        expression_statement, ///< Expression statement: x * 2;
        number,               ///< Numeric literal:      42
        variable,             ///< A variable name:      x
        variable_ref,         ///< A reference to a variable name node.
        subtraction,          ///< Subtraction:          x - 2
        addition,             ///< Addition:             x + 5
        multiplication,       ///< Multiplication:       x * y
        division,             ///< Division:             x / y
        equal_to,             ///< Equal to              x = y
        not_equal,            ///< Not equal             x != y
        greater_than,         ///< Greater than          x > y
        greater_or_equal,     ///< Greater or equal      x >= y
        less_than,            ///< Less than             x < y
        less_or_equal,        ///< Less or equal         x <= y
        logical_and,          ///< Logical logical_and           x and y
        logical_or,           ///< Logical logical_or            x or y
        logical_and_then,     ///< Logical logical_and THEN      x and then y
        logical_or_else,      ///< Logical logical_or ELSE       x or else y
        modulus,              ///< Modulus:              x % y
        unary_minus,          ///< Unary minus:          -x
        unary_plus,           ///< Unary plus:           +x
        function_call         ///< Function call:        abs(x)
    >;

/// Parse tree node is the same as a PEGTL basic node, but adds
/// the variant described above to differentiate the types of nodes.
struct node : public tao::pegtl::parse_tree::basic_node<node>
{
    using inherited = tao::pegtl::parse_tree::basic_node<node>;
    node() = default;
    node(node &&) = default;
    node(const node&) = delete;

    ~node() = default;

    node& operator=(node &&) = default;
    node& operator=(const node &) = delete;

    template <typename U>
    void set_kind( U&& u) noexcept
    {
        if constexpr (is_valid<U, node_kind>::value) {
            kind_ = std::forward<U>(u);
        } else {
            kind_ = std::monostate{};
        }
    }

    node_kind kind_;
};

using Ptr = std::unique_ptr<node>;

} // namespace Calc

#endif // NODE_H_INCLUDED
