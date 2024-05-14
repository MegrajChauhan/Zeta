/*
  MIT License

  Copyright (c) 2024 MegrajChauhan

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef _SEMA_
#define _SEMA_

#include "parser.hpp"
#include "symtable.hpp"
#include "error.hpp"
#include <vector>
#include <string>
#include <unordered_map>

// Unlike parser and like lexer, Sema will go through every
#define _set_() state = error::_ERR_

namespace zeta
{
    namespace sema
    {
        class Sema
        {
            std::vector<std::unique_ptr<nodes::Node>> nodes;      // the parsed nodes
            std::vector<std::unique_ptr<nodes::Node>> inst_nodes; // the instruction nodes
            symtable::SymTable symtable;                          // the symbol table
            lexer::Lexer &_l;
            size_t main_proc_index = 0;
            error::_STATES_ state = error::_NORMAL_;

        public:
            Sema(lexer::Lexer &l, parser::Parser &p) : _l(l)
            {
                p.move_nodes(nodes);
            }

            error::_STATES_ analyse();

            void gen_symtable();

            void validate_symbols();

            void pass_nodes(std::vector<std::unique_ptr<nodes::Node>> &dest)
            {
                dest = std::move(inst_nodes);
            }

            auto get_symtable() { return symtable; }

            // this makes sure that all the procedures have been defined
            void check_proc_declr();

            size_t get_main_declr_pos() { return main_proc_index; }
        };
    };
};

#endif