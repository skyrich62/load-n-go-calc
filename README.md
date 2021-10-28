# load-n-go-calc
A load and go parser implementing simple scripting calculations using PEGTL.

This is a simple project to demonstrate the ease of use of the PEGTL.

Specifically, I wrote this to demonstrate an answer to PEGTL issue #248: https://github.com/taocpp/PEGTL/issues/248.  As such, it is not intended as a
complete solution, but rather an exercise to point out how to use a
std::variant in the PEGTL parse tree node and a "cleaner way(tm)" of visiting
the different variants.

The input is currently only on the command line, and consists of compound
statements, assignment statements, if-statements, block statements,
declaration statements, loop statements, exit statements, return statements,
function definition statements, or expression statements.

## Assignment-statements have the form:

    variable := expression;

## Expression-statements have the form:

    expression;

## If-statements have the form:

    if (condition) statement [else statement]

The "else" clause is optional.  The condition is any expression.  If the result is 0 the condition is false, otherwise it is true.

## Declaration-statemetns have the form:

    var variableName;

Variables may be used without being defined, (in which case, they are implicitly defined.)

## Loop-statements have two forms:
### Top test loop statements have the form:

   loop while expression Compound-statement
   loop until expression Compound-statement

### Bottom test loop statements have the form:

    loop Compound-statement while expression
    loop Compound-statement until expression

If the loop-statement contains a "while" clause, the loop will continue as long as the expression evalutates to a non-zero value.
If the loop-statement contains an "until" clause, the loop will continue until the expression evaluates to a non-zero value.
Top-test loops will never iterate, bottom-test loop statements will always iterate at least once.

## Exit-statements have the form:

    exit;
    exit if expression;


 If there is an "if" clause, and it's expression evaluates to a non-zero value, then the immediately enclosing loop will be terminated.
 If there is no "if" clause, then the immediately enclosing loop will be terinated.
 Exit-statements are only allowed inside the body of a loop statement.  An error message will be put out otherwise.

## Compound-statements have the form:

    { statement; statement; statement; ... }

Only integer types, (c++ "int") are understood for now.  Variables start with a letter and may be followed by any number of letters and/or digits.
Variables defined inside a compound statement have scope only within the compound statement.  A variable may be explicitly defined inside a compound statement which has the same name/identifier as one in a surrounding scope.  This effectively hides the outer scope variable while inside the compound statement.
Compound statements can optionally have a name.  The name is parsed, but it currently is not put into the resulting parse tree.  This is for future use.

    xyzzy: {statement; statement; ... }

Here, "xyzzy" is the compound statement name.  In the future, a scope operator will be introduced which will allow access to hidden variable names by explictly designating the scope.

## Return statements have the form:

    return expression;

Return statements are only allowed inside function definitions, (see below).

## Function definitions have the form:

    def func(param1, param2) Compound-statement

There may be any number of parameters.  Recursion is not fully supported.
(There are no stack frames during evaluation.)

### Function example

    def fac(n) {
        var result;
        result := 1;
        loop while (n > 1) {
            result := result * n;
            n := n - 1;
        }
    }

    var x;
    x := fac(4);

Function "fac(n)" computes the factorial of "n".  In this case the factorial
of 4, (24), should be assigned to variable "x".

# Example usage

    calc file.calc

Where the contents of "file.calc" are:

   a := 5 * 2;
   b := a + 4;

The two statements will be parsed, and the resulting parse tree will be printed out to a file named: "calc-parse.dot" in GraphViz "dot" notation.
Then, semantic analysis will be performed, to create an AST from the resulting parse tree.
The AST will be putput to a file named: "calc-ast.dot", (also in "dot" notation).
Variable 'a' will be assigned the value 10, and variable 'b' will be assigned the value 14.


Thus the output, (combined std::cout and std::cerr) of the above command-line looks like this:

    $ calc file.calc
    Parse successful.
    Result: a = 10
    Result: b = 14

When an assignment statement has been evaluated, the result is displayed as shown above, with the variable name, and the expression value.
When an expression statement has been evaluated, just the expression value is displayed.
There is no display for the other statement types.

# Operators

The following operators are understood:
* "+", unary and binary plus / addition, (e.g. +a; and a + b;)
* "-", unary and binary minus / subtraction, (e.g. -a; and a - b;)
* "*", multiplication
* "/", division
* "%", modulus
* "<", less than
* ">", greater than
* ">=", greater then or equal to
* "<=", less than or equal to
* "=", equal to, (not to be confused with assignment, ":="
* "!=", not equal to
* "and", logical and.  Both the right- and left- side are evaluated. (see "and then")
* "or", logical or.  Both the right- and left- side are evaluated. (see "or else")
* "not", logical not.
* "and then", short circuiting logical and if the left side is 0, the right side is not evaluated.
* "or else", short circuiting logical or.  If the left side is non-zero, the right side is not evaluated.

# Intrinsic functions

There are two intrinsic functions, "abs", and "sgn".

* abs(n), return the absolute value of the argument.
* sgn(n), return the sign of the argument.  (-1, 0, or 1, if the argument is negative, zero, or positive, respectively).

