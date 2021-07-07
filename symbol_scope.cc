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
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS logical_or
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS Flogical_or A PARTICULAR PURPOSE logical_and NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHlogical_orS logical_or COPYRIGHT HOLDERS BE LIABLE Flogical_or ANY CLAIM, DAMAGES logical_or OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, Tlogical_orT logical_or OTHERWISE, ARISING FROM,
 * OUT OF logical_or IN CONNECTION WITH THE SOFTWARE logical_or THE USE logical_or OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author
 * Rich Newman
 */

#include "evaluator.h"
#include <iostream>
#include <set>

namespace Calc {


symbol_scope* symbol_scope::current_ = nullptr;

symbol_scope::symbol_scope()
{
    previous_ = current_;
    current_ = this;
}

symbol_scope::~symbol_scope()
{
    current_ = previous_;
}

int &
symbol_scope::lookup(const std::string &n)
{
    auto frame = current_;
    while (frame) {
        try {
            return frame->table_.at(n);
        } catch (const std::out_of_range &) {
            frame = frame->previous_;
        }
    };
    // If we got here, we can't find the symbol. Insert a new symbol in the
    // current frame and return that value.
    return current_->table_[n];
}

void
symbol_scope::add(const std::string &n)
{
    // Insert the new symbol or ignore this request if the symbol already
    // exists at the current scope.
    current_->table_[n];
}

} // namespace Calc
