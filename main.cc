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

#include <CompuBrite/CheckPoint.h>
#include <iostream>
#include <fstream>

static void print_dot(const std::string &name, Calc::Node::node &root)
{
    std::ofstream os(name);
    Calc::print_dot(os, root);
}

int main(int argc, char *argv[])
{
     using namespace tao::pegtl;
     namespace cbi = CompuBrite;

     if (argc == 1) {
        std::cerr << "usage: calc <statements>\n";
        return 1;
     }
     for (auto i = 1u; i < argc; ++i) {
        try {
            cbi::CheckPoint trace("trace");
            if (trace.active()) {
                argv_input in(argv, i);
                complete_trace<Calc::grammar::grammar>(in);
            }
            argv_input in(argv, i);
            auto root = parse_tree::parse<Calc::grammar::grammar, Calc::Node::node, Calc::grammar::selector>(in);
            if (root) {
                std::cerr << "Parse successful." << std::endl;
                root->set_kind<Calc::Node::root>({nullptr});
                std::ofstream os("calc-parse.dot");
                parse_tree::print_dot(os, *root);
                //print_dot("calc-parse.dot", *root);
                {
                    auto parent = root->get_kind<Calc::Node::root>();
                    Calc::semantic_analysis sem(*parent);
                    Calc::traversal trav(sem, Calc::node_visitor::PRE_VISIT |
                                              Calc::node_visitor::POST_VISIT);
                    trav.traverse(*root);
                }

                print_dot("calc-ast.dot", *root);
                Calc::evaluator eval;
                eval.accept(*root);
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
