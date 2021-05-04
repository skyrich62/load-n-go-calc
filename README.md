# load-n-go-calc
A load and go parser implementing simple 4-way calculations using PEGTL.

This is a simple project to demonstrate the ease of use of the PEGTL.

Specifically, I wrote this to demonstrate an answer to PEGTL issue #248: https://github.com/taocpp/PEGTL/issues/248.  As such, it is not intended as a
complete solution, but rather an exercise to point out how to use a
std::variant in the PEGTL parse tree node and a "cleaner way(tm)" of visiting
the different variants.

The input is currently only on the command line, and consists of simple
statements, either an assignment statement, or an expression statement.

Assignment statements have the form:

    variable := expression;

Expression statements have the form:

    expression;

# Example

    calc 'a := 5 * 2; b := a + 4;'

The two statements will be parsed, and the resulting parse tree will be evaluated.  Variable 'a' will be assigned the value 10, and variable 'b' will be assigned the value 14.

The parse tree itself will be output to std::cout in Graphviz "dot" notation, while the results of the evaluation will be output to std::cerr:

Thus the output, (combined std::cout and std::cerr) of the above command-line looks like this:

   $ calc 'a := 5 * 2; b := a + 4;'
   Parse successful.
   digraph parse_tree
   {
     x0xc3ec20 [ label="ROOT" ]
     x0xc3ec20 -> { x0xc3f100, x0xc3f500 }
     x0xc3f100 [ label="assignment_statement" ]
     x0xc3f100 -> { x0xc3f1b0, x0xc3f870 }
     x0xc3f1b0 [ label="tao::pegtl::ascii::identifier\n\"a\"" ]
     x0xc3f870 [ label="multiplication" ]
     x0xc3f870 -> { x0xc3f5b0, x0xc3f7c0 }
     x0xc3f5b0 [ label="integer\n\"5\"" ]
     x0xc3f7c0 [ label="integer\n\"2\"" ]
     x0xc3f500 [ label="assignment_statement" ]
     x0xc3f500 -> { x0xc3f260, x0xc3fbe0 }
     x0xc3f260 [ label="tao::pegtl::ascii::identifier\n\"b\"" ]
     x0xc3fbe0 [ label="addition" ]
     x0xc3fbe0 -> { x0xc3f9d0, x0xc3fc90 }
     x0xc3f9d0 [ label="tao::pegtl::ascii::identifier\n\"a\"" ]
     x0xc3fc90 [ label="integer\n\"4\"" ]
   }
   Result: a = 10
   Result: b = 14

