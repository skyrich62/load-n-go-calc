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

struct compound_statement
{
    std::unique_ptr<node> scope_;
};

struct root
{
    std::unique_ptr<node> scope_;
};

#define xx(a) struct a { };
#define yy(a)
#include "node_kind.def"
#undef xx
#undef yy

/// The variant which will hold information specific to the type of parse
/// tree node.
using node_kind =
    std::variant <
        std::monostate,       ///< Not classified yet.
#define xx(a) a,
#define yy(a) xx(a)
#include "node_kind.def"
#undef xx
#undef yy
        std::monostate        ///< Terminate the variant type list.
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
