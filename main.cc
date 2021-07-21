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

#include <tao/pegtl/contrib/trace.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>


#include "grammar.h"
#include "node.h"
#include "evaluator.h"
#include "traversal.h"
#include "semantic_analysis.h"
#include "selector.h"
#include "dotter.h"

#include <iostream>

int main(int argc, char *argv[])
{
     using namespace tao::pegtl;

     if (argc == 1) {
        std::cerr << "usage: calc <statements>\n";
        return 1;
     }
     for (auto i = 1u; i < argc; ++i) {
        try {
            argv_input in(argv, i);
            auto root = parse_tree::parse<Calc::grammar::grammar, Calc::Node::node, Calc::grammar::selector>(in);
            if (root) {
                std::cerr << "Parse successful." << std::endl;
                Calc::print_dot(std::cout, *root);
                Calc::semantic_analysis sem;
                Calc::traversal trav(sem);
                trav.traverse(*root);
                Calc::print_dot(std::cout, *root);
                //Calc::evaluator eval;
                //eval.accept(*root);
            } else {
                std::cerr << "Parse fail." << std::endl;
                return 1;
            }
        } catch (const std::exception &e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return 5;
        }
    }
    return 0;
}
