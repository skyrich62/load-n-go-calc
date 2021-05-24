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

/// ws <- space
struct ws    : space { };

/// wsp <- ws+
struct wsp   : plus< ws > { };

/// wss <- ws*
struct wss   : star< ws > { };

/// EQUAL <- '='
struct EQUAL : one< '=' > { };

/// COLON <- ':'
struct COLON : one< ':' > { };

/// ASSIGN <- COLON EQUAL
struct ASSIGN : seq< COLON, EQUAL > { };

/// PLUS <- '+'
struct PLUS   : one< '+' > { };

/// MINUS <- '-'
struct MINUS  : one< '-' > { };

/// STAR <- '*'
struct STAR   : one< '*' > { };

/// SLASH <- '/'
struct SLASH  : one< '/' > { };

/// PERCENT <- '%'
struct PERCENT : one< '%' > { };

/// LPAREN <- '('
struct LPAREN : one< '(' > { };

/// RPAREN <- ')'
struct RPAREN : one< ')' > { };

/// LBRACE <- '{'
struct LBRACE : one< '{' > { };

/// RBRACE <- '}'
struct RBRACE : one< '}' > { };

/// SEMI <- ';'
struct SEMI   : one< ';' > { };

/// unary_plus <- PLUS
struct unary_plus : PLUS { };

/// unary_minus <- MINUS
struct unary_minus: MINUS { };

/// addition <- PLUS
struct addition : PLUS { };

/// subtraction <- MINUS
struct subtraction : MINUS { };

/// multiplication <- STAR
struct multiplication : STAR { };

/// division <- SLASH
struct division : SLASH { };

/// modulus <- PERCENT
struct modulus : PERCENT { };

/// unary_adding_operator <- unary_plus / unary_minus
struct unary_adding_operator : sor< unary_plus, unary_minus > { };

/// binary_adding_operator <- addition / subtraction
struct binary_adding_operator : sor< addition, subtraction > { };

/// multiplying_operator <- multiplucation / division / modulus
struct multiplying_operator : sor< multiplication, division, modulus > { };

/// IF <- if !identifier_other
struct IF : keyword< 'i', 'f' > { };

/// ELSE <- else !identifier_other
struct ELSE : keyword< 'e', 'l', 's', 'e' > { };

/// keywords <- IF / ELSE
struct keywords: sor< IF, ELSE > { };

struct expression;
struct factor;
struct term;


/// integer <- digit+
struct integer : plus< digit > { };

/// function_call <- identifier LPAREN expression RPAREN
struct function_call :
    seq< identifier, wss, LPAREN, wss, expression, wss, RPAREN > { };

/// assignment <- identifier ASSIGN expression
struct assignment : seq< identifier, wss, ASSIGN, wss, expression > { };

/// expression <- unary_adding_operator? term (binary_adding_operator term)*
struct expression : seq<
                      opt< unary_adding_operator, wss >,
                      list< term, binary_adding_operator, ws >
                    > { };

/// term <- factor (multiplying_operator factor)*
struct term : list< factor, multiplying_operator, ws > { };

/// parenthezised_expr <- LPAREN expression RPAREN
struct parenthesized_expr : seq< LPAREN, wss, expression, wss, RPAREN > { };

/// factor <- function_call / integer / identifier / parenthesized_expr
struct factor :
    sor< function_call, integer, identifier, parenthesized_expr > { };

struct statement;

/// if_statement <- IF '(' expression ')' statement (ELSE statement)?
struct if_statement :
    seq<
        IF, wss,
        LPAREN, wss,
        expression, wss,
        RPAREN, wss,
        statement,
        opt< wss, ELSE, wss, statement >
    > { };

/// assignment_statement <- assignment
struct assignment_statement : seq< assignment, wss, SEMI > { };

/// expression_statement < expression
struct expression_statement : seq< expression, wss, SEMI > { };

struct statement;

/// simple_statement <- (assignment_statement / expression_statement / if_statement) SEMI
struct simple_statement :
    sor<
        if_statement,
        assignment_statement,
        expression_statement
    > { };

/// compound_statement <- '{' statement* '}'
struct compound_statement :
    seq<
        LBRACE, wss,
        star< statement, wss >, wss,
        RBRACE
    > { };

/// statement <- compound_statement / simple_statement
struct statement : sor< compound_statement, simple_statement > { };

/// grammar <- statement+ eolf
struct grammar : seq< wss, plus< statement, wss >, eolf > { };


/// Useful for using lambda expressions in function overloads for use with
/// std::visit.  See the example code here:
///     https://en.cppreference.com/w/cpp/utility/variant/visit
template< class... Ts > struct overloaded : Ts... { using Ts::operator()...; };
template< class... Ts > overloaded(Ts...) -> overloaded<Ts...>;


/// Define the members of the variant in the parse tree nodes.

/// A node which holds a number, (integer).  For now, this only holds the
/// value, but can be modified to hold addtional information, ( e.g. how
/// the number was parsed, "011" vs "9" for instance).
struct number
{
    int _value;
};

/// A node which holds a symbol.  Again, for now, this is only the name
/// of the symbol, but other information could be placed here.
struct symbol
{
    std::string _value;
};

/// The variant which will hold information specific to the type of parse
/// tree node.
using node_kind =
    std::variant <
        std::monostate,       ///< Placeholder.
        if_statement,         ///< if statement:         if (x) stm;
        compound_statement,   ///< Compound statement:   { statement+ }
        assignment_statement, ///< Assignment statement: x:= 5;
        expression_statement, ///< Expression statement: x;
        number,               ///< Numeric literal:      42
        symbol,               ///< Symbol:               x
        subtraction,          ///< Subtraction:          x - 2
        addition,             ///< Addition:             x + 5
        multiplication,       ///< Multiplication:       x * y
        division,             ///< Division:             x / y
        modulus,              ///< Modulus:              x % y
        unary_minus,          ///< Unary minus:          -x
        unary_plus,           ///< Unary plus:           +x
        function_call         ///< Function call:        abs(x)
    >;

/// Parse tree node is the same as a PEGTL basic node, but adds
/// the variant described above to differentiate the types of nodes.
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

/// Rearrange expressions to remove "factor", "term", and expression
/// nodes, and put the tree in prefix form.
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

/// Store a literal number, (integer), in the parse tree.
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

/// Store a symbol in the parse tree.
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

/// Attempt to set the std::variant according to the type provided.
template<typename Rule>
bool try_type(Ptr &n)
{
    if (n->is_type<Rule>()) {
        n->kind = Rule();
        return true;
    }
    return false;
}


/// Remove the content from the node, and attempt to classify its
/// type.
struct remove_content : parse_tree::apply<remove_content>
{
    template< typename... States>
    static void transform( Ptr &n, States&&... st)
    {
        try_type<function_call>(n)        ||
        try_type<if_statement>(n)         ||
        try_type<assignment_statement>(n) ||
        try_type<assignment_statement>(n) ||
        try_type<expression_statement>(n) ||
        try_type<addition>(n)             ||
        try_type<subtraction>(n)          ||
        try_type<multiplication>(n)       ||
        try_type<modulus>(n)              ||
        try_type<division>(n)             ||
        try_type<unary_plus>(n)           ||
        try_type<unary_minus>(n);
        n->remove_content();
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

  /// for identifiers, create a symbol node.
  store_symbol::on< identifier >,

  /// Remove the content and classify the nodes for these.
  remove_content::on<
    if_statement,
    assignment_statement,
    expression_statement,
    compound_statement,
    addition,
    subtraction,
    multiplication,
    division,
    modulus,
    unary_plus,
    unary_minus,
    function_call
  >,

  /// Rearrange the expression sub-tree nodes.
  rearrange_expr::on<
    factor,
    expression,
    term
  >
>;

/// Evaluate the parse tree.
/// Once the parse has completed, traverse the parse tree evaluating the nodes to
/// produce a result.
class evaluator
{
public:
    evaluator() = default;
    evaluator(const evaluator &) = delete;
    evaluator(evaluator &&) = default;
    ~evaluator() = default;

    evaluator& operator=(const evaluator &) = delete;
    evaluator& operator=(evaluator &&) = default;

    /// Visit a node.
    int visit(const node&);

    /// Visit a node based on its kind.
    int visit(const node &, const node_kind &);

    /// Visit an if statement.  Evaluate the expression, if the result it non-zero,
    /// evaluate the children.
    int visit(const node &, const if_statement &);

    /// Visit an assignment statement.  Evaluate the RHS, and assign the result
    /// to the given identifier, (LHS).
    int visit(const node &, const assignment_statement &);

    /// Visit an expression statement.  Evaluate the expression and print the result.
    int visit(const node &, const expression_statement &);

    /// Visit an addition node. Evalate the LHS, evaluate the RHS, add them together,
    /// and return the result.
    int visit(const node &, const addition &);

    /// Visit a subtraction node. Evaluate the LHS and RHS, subtract the
    /// RHS from the LHS, return the result.
    int visit(const node &, const subtraction &);

    /// Visit a multiplication node. Evalute the LHS and RHS, multiply them, return
    /// the result.
    int visit(const node &, const multiplication &);

    /// Visit a division node.  Evaluate the LHS and RHS, divide LHS by RHS.
    int visit(const node &, const division &);

    /// Visit a modulus node.  Evaluate the LHS % RHS
    int visit(const node &, const modulus &);

    /// Visit a unary_minus node. Evaluate the operand, negate it and return the result.
    int visit(const node &, const unary_minus &);

    /// Visit a unary_plus node.  Evaluate the operand, and return it.
    int visit(const node &, const unary_plus &);

    /// Visit a symbol, (identifier), node.  Store the symbol name in the node,
    /// Return the value of the symbol from the symbol table.  If the symbol has
    /// never before been seen, set it's value to zero.
    int visit(const node &, const symbol &);

    /// Visit a number, (integer), node.  Store the binary value of the integer in
    /// the node.
    int visit(const node &, const number &);

    /// Visit a function_call node.  If the function is not known, return 0.
    /// Otherwise, evaluate the operand expression, apply the function and return
    /// the value.
    /// @note
    /// Currently, the only known functions are:
    ///  "abs" for absolute value.
    ///  "sgn" returns -1, 0, or 1, depending on the sign of the operand.
    int visit(const node &, const function_call &);

private:
    /// The symbol table.  Map the symbol names to their values.
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
        [this, &n](const if_statement& is)         { return this->visit(n, is); },
        [this, &n](const assignment_statement& as) { return this->visit(n, as); },
        [this, &n](const expression_statement& es) { return this->visit(n, es); },
        [this, &n](const addition &add)            { return this->visit(n, add); },
        [this, &n](const subtraction &sub)         { return this->visit(n, sub); },
        [this, &n](const multiplication &mul)      { return this->visit(n, mul); },
        [this, &n](const division &div)            { return this->visit(n, div); },
        [this, &n](const modulus &div)             { return this->visit(n, div); },
        [this, &n](const unary_minus &um)          { return this->visit(n, um); },
        [this, &n](const unary_plus &up)           { return this->visit(n, up); },
        [this, &n](const symbol &sym)              { return this->visit(n, sym); },
        [this, &n](const number  &i)               { return this->visit(n, i); },
        [this, &n](const function_call  &f)        { return this->visit(n, f); },
        [this, &n](auto arg)
        {
            auto res = 0;
            auto &c = n.children;
            for (const auto &child: c) {
                res = this->visit(*child);
            }
            return res;
        }
      }, kind);
}

int
evaluator::visit(const node &n, const if_statement &)
{
    const auto &cond = *n.children[0];
    auto res = visit(cond);
    if (res != 0) {
        visit(*n.children[1]);
    } else {
        if (n.children.size() == 3) {
            visit(*n.children[2]);
        }
    }
    return 0;
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
evaluator::visit(const node &n, const modulus &)
{
    return visit(*n.children[0]) % visit(*n.children[1]);
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
    auto operand = visit(*n.children[1]);
    auto func = std::get<symbol>(n.children[0]->kind)._value;
    if (func == "abs") {
        return abs(operand);
    } else if (func == "sgn") {
        if (operand < 0) {
            return -1;
        } else if (operand > 0) {
            return 1;
        }
        return 0;
    }
    std::cerr << "Unknown function: " << func << " -- ignored.\n";
    return operand;
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
