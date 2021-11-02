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

#ifndef GRAMMAR_H_INCLUDED
#define GRAMMAR_H_INCLUDED

#include <tao/pegtl.hpp>

namespace Calc::grammar {
using namespace TAO_PEGTL_NAMESPACE;

/// comment <- // .* eolf
struct comment :
    seq<
        two< '/' >,
        until< eolf >
    > { };

/// ws <- space / comment
struct ws    : sor< space, comment > { };

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

/// LANGLE <- '<'
struct LANGLE : one< '<' > { };

/// RANGLE <- '}'
struct RANGLE : one< '>' > { };

/// SEMI <- ';'
struct SEMI   : one< ';' > { };

/// BANG <- '!'
struct BANG   : one< '!' > { };

/// COMMA <- ','
struct COMMA  : one< ',' > { };

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

/// greater_than <- RANGLE !EQUAL
struct greater_than : seq< RANGLE, not_at< EQUAL > > { };

/// greater_or_equal < - RANGLE EGUAL
struct greater_or_equal : seq< RANGLE, EQUAL > { };

/// less_than <- LANGLE !EQUAL
struct less_than : seq< LANGLE, not_at< EQUAL > > { };

/// less_or_equal <- LANGLE EQUAL
struct less_or_equal : seq< LANGLE, EQUAL > { };

/// equal_to <- EQUAL
struct equal_to : EQUAL { };

/// not_equal <- BANG EQUAL
struct not_equal : seq< BANG, EQUAL > { };

/// unary_adding_operator <- unary_plus / unary_minus
struct unary_adding_operator : sor< unary_plus, unary_minus > { };

/// binary_adding_operator <- addition / subtraction
struct binary_adding_operator : sor< addition, subtraction > { };

/// multiplying_operator <- multiplication / division / modulus
struct multiplying_operator : sor< multiplication, division, modulus > { };

/// relational_operator <- equal_to / not_equal / greater_than /
///                        greater_or_equal / less_than / less_or_equal
struct relational_operator :
    sor<
        equal_to, not_equal,
        greater_or_equal, greater_than,
        less_or_equal, less_than
    > { };


/// IF <- if !identifier_other
struct IF : keyword< 'i', 'f' > { };

/// ELSE <- else !identifier_other
struct ELSE : keyword< 'e', 'l', 's', 'e' > { };

/// EXIT <- exit !identifier_other
struct EXIT : keyword< 'e', 'x', 'i', 't' > { };

/// RETURN <- return !identifier_other
struct RETURN : keyword< 'r', 'e', 't', 'u', 'r', 'n' > { };

/// THEN <- then !identifier_other
struct THEN : keyword< 't', 'h', 'e', 'n' > { };

/// OR <- or !identifier_other
struct ORkw : keyword< 'o', 'r' > { };

/// AND <- and !identifier_other
struct ANDkw : keyword< 'a', 'n', 'd' > { };

/// VAR <- var !identifier_other
struct VAR : keyword< 'v', 'a', 'r' > { };

/// DEF <- def !identifier_other
struct DEF : keyword< 'd', 'e', 'f' > { };

/// LOOP <- loop !identifier_other
struct LOOP : keyword< 'l', 'o', 'o', 'p' > { };

/// NOT <- not !identifier_other
struct NOT : keyword< 'n', 'o', 't' > { };

/// UNTIL <- until !identifier_other
struct UNTIL : keyword < 'u', 'n', 't', 'i', 'l' > { };

/// WHILE <- while !identifier_other
struct WHILE : keyword < 'w', 'h', 'i', 'l', 'e' > { };

/// AND <- ANDkw !(wsp THEN)
struct AND : seq< ANDkw, not_at< wsp, THEN > > { };

/// AND_THEN <- ANDkw THEN
struct AND_THEN : seq< ANDkw, wsp, THEN > { };

/// OR <- ORkw !(wsp ELSE)
struct OR : seq< ORkw, not_at< wsp, ELSE > > { };

/// OR_ELSE
struct OR_ELSE : seq< ORkw, wsp, ELSE > { };

/// keywords <- AND / DEF / ELSE / EXIT / IF / LOOP / NOT / OR / THEN /
///             UNTIL / VAR / WHILE / RETURN
struct keywords:
    sor< AND, DEF, ELSE, EXIT, IF, LOOP, NOT, OR, RETURN, THEN,
         UNTIL, VAR, WHILE > { };

/// logical_operator <- OR / AND / AND_THEN / OR_ELSE
struct logical_operator : sor< AND_THEN, OR_ELSE, OR, AND > { };

/// non_kw_ident <- !keywords  identifier
struct non_kw_ident : seq< not_at< keywords >, identifier > { };

/// symbol_name <- non_kw_ident
struct symbol_name : seq< non_kw_ident > { };

/// block_id <- non_kw_ident
struct block_id : seq< non_kw_ident > { };

struct relation;
struct expression;
struct factor;
struct term;


/// integer <- digit+
struct integer : plus< digit > { };

/// expression_list <- ( expression , )*
struct expression_list : list<expression, COMMA, ws> { };

/// function_call <- symbol_name LPAREN expression RPAREN
struct function_call :
    seq< symbol_name, wss, LPAREN, wss, expression_list, wss, RPAREN > { };

/// assignment <- symbol_name ASSIGN expression
struct assignment : seq< symbol_name, wss, ASSIGN, wss, expression > { };

/// expression <- relation (wss, logical_operator)*
struct expression : list< relation, logical_operator, ws> { };

/// simple_expression <- unary_adding_operator? term (binary_adding_operator term)*
struct simple_expression :
    seq<
        opt< unary_adding_operator, wss >,
        list< term, binary_adding_operator, ws >
    > { };

/// term <- factor (multiplying_operator factor)*
struct term : list< factor, multiplying_operator, ws > { };

/// parenthezised_expr <- LPAREN expression RPAREN
struct parenthesized_expr : seq< LPAREN, wss, expression, wss, RPAREN > { };

/// primary <- function_call / integer / symbol_name / parenthesized_expr
struct primary :
    sor< function_call, integer, symbol_name, parenthesized_expr> { };

/// logical_negation <- NOT primary
struct logical_negation: seq <NOT, wsp, primary> { };

/// factor <- logical_negation / primary
struct factor : sor< logical_negation, primary> { };


/// relation <- simple_expression (relational_operator wss simple_expression)*
struct relation : list< simple_expression, relational_operator, ws> { };

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

/// assignment_statement <- assignment ';'
struct assignment_statement : seq< assignment, wss, SEMI > { };

/// expression_statement <- expression ';'
struct expression_statement : seq< expression, wss, SEMI > { };

/// decl_statement <- VAR symbol_name ';'
struct decl_statement : seq< VAR, wsp, symbol_name, wss, SEMI > { };

/// exit_statement <- EXIT (IF expression)? ';'
struct exit_statement :
    seq< EXIT, opt< wsp, block_id>, opt< wsp, IF, wsp, expression >, wss, SEMI> { };

/// return_statement <- RETURN expression ';'
struct return_statement :
    seq<RETURN, wsp, expression, wss, SEMI> { };
struct function_definition;

/// simple_statement <- (decl_statement / assignment_statement / expression_statement / if_statement) SEMI
struct simple_statement :
    sor<
        function_definition,
        decl_statement,
        exit_statement,
        return_statement,
        return_statement,
        if_statement,
        assignment_statement,
        expression_statement
    > { };

/// block_name <- block_id COLON
struct block_name : seq< block_id, wss, COLON, wss > { };

/// compound_statement <- block_name? '{' statement* '}'
struct compound_statement :
    seq<
        opt< block_name >,
        LBRACE, wss,
        star< statement, wss >, wss,
        RBRACE
    > { };

/// function_definition <- function_name '(' parameters ')' '{' statement* '}'
struct function_definition :
    seq<
      DEF, wss,
      symbol_name, wss,
      LPAREN, wss,
      list< symbol_name, COMMA, ws>, wss,
      RPAREN, wss,
      compound_statement
    > { };

/// while_test <- WHILE expression
struct while_test :
    seq< WHILE, wss, LPAREN, wss, expression, wss, RPAREN >
    { };

/// until_test <- WHILE expression
struct until_test :
    seq< UNTIL, wss, LPAREN, wss, expression, wss, RPAREN >
    { };

/// top_test <- WHILE / UNTIL expression compound statement
struct top_test :
    seq< sor< while_test, until_test >, wsp, compound_statement > { };

struct bottom_test :
    seq< compound_statement, wsp, sor< while_test, until_test>, wss, SEMI > { };

/// loop_statement <- LOOP (top_test / bottom_test)
/// A loop may have a loop_test at the top, or at the bottom, but not
/// both.
struct loop_statement :
    seq<
      LOOP, wsp, sor < top_test, bottom_test >
    > { };

/// statement <- loop_statement / compound_statement / simple_statement
struct statement :
    sor< loop_statement, compound_statement, simple_statement > { };

/// grammar <- statement+ eolf
struct grammar : seq< wss, plus< statement, wss >, eof > { };

} // namespace Calc

#endif // GRAMMAR_H_INCLUDED
