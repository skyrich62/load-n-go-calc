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
#include <functional>

template< typename T, typename U> struct is_valid;

template< typename T, typename... Ts>
struct is_valid<T, std::variant< Ts... > > :
std::bool_constant< (std::is_same_v< T, Ts > || ...) >
{ };

namespace Calc::Node {
struct node;

/// Define the base members of the variant in the parse tree nodes.

/// Something that has scope, currently only the root, and a compound
/// statement.
struct parent
{
    std::unique_ptr<node> scope_;
};

/// A constant value.
struct value {
    int value_ = 0;
};

/// A reference to a symbol or variable.
struct symbol_ref {
    node *variable_ = nullptr;
};

/// An operation of some kind.
struct operation { };

/// A statement of some kind.
struct statement { };

/// A parent statement, (used for compound statements).
struct parent_stmt : public parent, public statement { };

/// A variable/symbol name node.
struct symbol_name {
    std::string name_;
};

/// A scope node
struct scope_base {
    /// The parent scope of this one, or nullptr for the top-level, (global)
    /// scope.
    node *parent_scope_ = nullptr;

    /// Subscopes of this scope if any.
    std::vector<node *> subscopes_;
};

/// A function node kind
struct function_base : public symbol_name, public scope_base {
    std::function<int(int)> intrinsic_;
};

/// Used as a sentinel to end the list of variants.
struct node_kind_last { };

#define xx(a, b) struct a : public b { };
#include "node_kind.def"

/// The variant which will hold information specific to the type of parse
/// tree node.
using node_kind =
    std::variant <
        std::monostate,       ///< Not classified yet.
#define xx(a, b) a,
#include "node_kind.def"
        node_kind_last
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
