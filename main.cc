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

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/trace.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include <variant>
#include <sstream>
#include <map>
#include <iostream>

using namespace TAO_PEGTL_NAMESPACE;

struct ws    : space { };
struct wsp   : plus< ws > { };
struct wss   : star< ws > { };

struct EQUAL : one< '=' > { };
struct COLON : one< ':' > { };
struct ASSIGN : seq< COLON, EQUAL > { };
struct PLUS   : one< '+' > { };
struct MINUS  : one< '-' > { };
struct STAR   : one< '*' > { };
struct SLASH  : one< '/' > { };
struct LPAREN : one< '(' > { };
struct RPAREN : one< ')' > { };
struct SEMI   : one< ';' > { };

struct unary_plus : PLUS { };
struct unary_minus: MINUS { };

struct addition : PLUS { };
struct subtraction : MINUS { };
struct multiplication : STAR { };
struct division : SLASH { };

struct unary_adding_operator : sor< unary_plus, unary_minus > { };
struct binary_adding_operator : sor< addition, subtraction > { };
struct multiplying_operator : sor< multiplication, division > { };


struct expression;
struct factor;
struct term;

struct integer : plus< digit > { };

struct function_call :
    seq< identifier, wss, LPAREN, wss, expression, wss, RPAREN > { };

struct assignment : seq< identifier, wss, ASSIGN, wss, expression > { };

struct expression : seq<
                      opt< unary_adding_operator, wss >,
                      list< term, binary_adding_operator, ws >
                    > { };

struct term : list< factor, multiplying_operator, ws > { };

struct parenthesized_expr : seq< LPAREN, wss, expression, wss, RPAREN > { };

struct factor :
    sor< function_call, integer, identifier, parenthesized_expr > { };


struct assignment_statement : assignment { };
struct expression_statement : seq< expression > { };

struct statement : seq< sor< assignment_statement, expression_statement >, wss, SEMI > { };

struct grammar : seq< wss, plus< statement, wss >, eolf > { };

template< class... Ts > struct overloaded : Ts... { using Ts::operator()...; };
template< class... Ts > overloaded(Ts...) -> overloaded<Ts...>;


struct number
{
    int _value;
};

struct symbol
{
    std::string _value;
};

using node_kind =
    std::variant <
        std::monostate,
        assignment_statement,
        expression_statement,
        number,
        symbol,
        subtraction,
        addition,
        multiplication,
        division,
        unary_minus,
        unary_plus,
        function_call
    >;

struct node : public tao::pegtl::parse_tree::basic_node<node>
{
    node() = default;
    node(node &&) = default;
    node(const node&) = delete;

    ~node() = default;

    node& operator=(node &&) = default;
    node& operator=(const node &) = delete;

    node_kind kind;
};

using Ptr = std::unique_ptr<node>;

struct rearrange_expr : parse_tree::apply<rearrange_expr>
{
    template <typename... States>
    static void transform( Ptr &n, States&&... st)
    {
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

struct store_number : parse_tree::apply<store_number>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        std::istringstream is(n->string());
        number val;
        is >> val._value;
        n->kind = val;
    }
};

struct store_symbol : parse_tree::apply<store_symbol>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        symbol val;
        val._value = n->string();
        n->kind = val;
    }
};

template<typename Rule>
bool try_type(Ptr &n)
{
    if (n->is_type<Rule>()) {
        n->kind = Rule();
        return true;
    }
    return false;
}

struct remove_content : parse_tree::apply<remove_content>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        try_type<function_call>(n)        ||
        try_type<assignment_statement>(n) ||
        try_type<expression_statement>(n) ||
        try_type<addition>(n)             ||
        try_type<subtraction>(n)          ||
        try_type<multiplication>(n)       ||
        try_type<division>(n)             ||
        try_type<unary_plus>(n)           ||
        try_type<unary_minus>(n);
        n->remove_content();
    }
};


template <typename Rule>
using selector = parse_tree::selector<
  Rule,
  store_number::on< integer >,
  store_symbol::on< identifier >,
  remove_content::on<
    assignment_statement,
    expression_statement,
    addition,
    subtraction,
    multiplication,
    division,
    unary_plus,
    unary_minus,
    function_call
  >,
  rearrange_expr::on<
    factor,
    expression,
    term
  >
>;

class evaluator
{
public:
    evaluator() = default;
    evaluator(const evaluator &) = delete;
    evaluator(evaluator &&) = default;
    ~evaluator() = default;

    evaluator& operator=(const evaluator &) = delete;
    evaluator& operator=(evaluator &&) = default;

    int visit(const node&);
    int visit(const node &, const node_kind &);
    int visit(const node &, const assignment_statement &);
    int visit(const node &, const expression_statement &);
    int visit(const node &, const addition &);
    int visit(const node &, const subtraction &);
    int visit(const node &, const multiplication &);
    int visit(const node &, const division &);
    int visit(const node &, const unary_minus &);
    int visit(const node &, const unary_plus &);
    int visit(const node &, const symbol &);
    int visit(const node &, const number &);
    int visit(const node &, const function_call &);

private:
    std::map<std::string, int> _symbol_table;
};

int
evaluator::visit(const node &n)
{
    if (n.is_root()) {
        for (const auto &child : n.children) {
            visit(*child);
        }
    } else {
        return visit(n, n.kind);
    }
    return 0;
}

int
evaluator::visit(const node &n, const node_kind &kind)
{
    return
      std::visit(overloaded {
        [this, &n](const assignment_statement& as) { return this->visit(n, as); },
        [this, &n](const expression_statement& es) { return this->visit(n, es); },
        [this, &n](const addition &add)            { return this->visit(n, add); },
        [this, &n](const subtraction &sub)         { return this->visit(n, sub); },
        [this, &n](const multiplication &mul)      { return this->visit(n, mul); },
        [this, &n](const division &div)            { return this->visit(n, div); },
        [this, &n](const unary_minus &um)          { return this->visit(n, um); },
        [this, &n](const unary_plus &up)           { return this->visit(n, up); },
        [this, &n](const symbol &sym)              { return this->visit(n, sym); },
        [this, &n](const number  &i)               { return this->visit(n, i); },
        [this, &n](const function_call  &f)        { return this->visit(n, f); },
        [](auto arg)                           { return 0;}
      }, kind);
}

int
evaluator::visit(const node &n, const assignment_statement &)
{
    auto res = visit(*n.children[1]);
    auto var = std::get<symbol>(n.children[0]->kind)._value;
    _symbol_table[var] = res;
    std::cerr << "Result: " << var << " = " << res << std::endl;
    return res;
}

int
evaluator::visit(const node &n, const expression_statement &)
{
    auto res = visit(*n.children[0]);
    std::cerr << "Result: " << res << std::endl;
    return res;
}

int
evaluator::visit(const node &n, const symbol &sym)
{
    return _symbol_table[sym._value];
}

int
evaluator::visit(const node &, const number &i)
{
    return i._value;
}

int
evaluator::visit(const node &n, const unary_minus &)
{
    return -1 * visit(*n.children[0]);
}

int
evaluator::visit(const node &n, const unary_plus &)
{
    return visit(*n.children[0]);
}

int
evaluator::visit(const node &n, const multiplication &)
{
    return visit(*n.children[0]) * visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const division &)
{
    return visit(*n.children[0]) / visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const addition &)
{
    return visit(*n.children[0]) + visit(*n.children[1]);
}

int
evaluator::visit(const node &n, const subtraction &)
{
    return visit(*n.children[0]) - visit(*n.children[1]);
}


int
evaluator::visit(const node &n, const function_call &f)
{
    auto func = std::get<symbol>(n.children[0]->kind)._value;
    if (func != "abs") {
        std::cerr << "Unknown function: " << func << " -- ignored.\n";
        return 0;
    }
    auto operand = visit(*n.children[1]);
    return abs(operand);
}

int main(int argc, char *argv[])
{
     if (argc == 1) {
        std::cerr << "usage: calc <statements>\n";
        return 1;
     }
     for (auto i = 1u; i < argc; ++i) {
        try {
            argv_input in(argv, i);
            auto root = parse_tree::parse<grammar, node, selector>(in);
            if (root) {
                std::cerr << "Parse successful." << std::endl;
                print_dot(std::cout, *root);
                evaluator eval;
                eval.visit(*root);
            } else {
                std::cout << "Parse fail." << std::endl;
                return 1;
            }
        } catch (const std::exception &e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return 5;
        }
    }
    return 0;
}
