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

#ifndef _PARSER_
#define _PARSER_

#include "lexer.hpp"
#include "error.hpp"
#include "nodes.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

/*
  This version of the assembler is making use of the previous assembler for parsing, semantic analysis and codegen.
  Improvements will be made, of course.
*/

#define set() err = true

namespace zeta
{
    namespace parser
    {
        enum CurrentSection
        {
            // anything must belong to a section else an error will be thrown
            _SECTION_NONE,
            _SECTION_DATA,
            _SECTION_TEXT,
        };

        class Parser
        {
            lexer::Lexer &lexer;
            CurrentSection section = _SECTION_NONE;
            std::vector<std::unique_ptr<nodes::Node>> nodes;
            zeta::lexer::Token curr_tok;
            bool encountered_text = false;
            bool err = false;

            void next_token()
            {
                curr_tok = lexer.next_token();
            }

        public:
            // lexer is already setup for us
            Parser(lexer::Lexer &_l) : lexer(_l) { curr_tok = lexer.next_token(); }

            // Unlike lexer, if the parser comes across any errors(including _TT_ERR_ from the lexer), it will terminate further parsing
            // While lexing, the lexer continues lexing to pinpoint every error.
            // In case of parsing, the successful assembling of the entire input depends on the successful parsing of every token
            // Hence, one error indicates that further parsing is pointless as any token being parsed in the future may be dependent on the successful parsing of current token.
            void parse();

            void send_errors(std::string msg)
            {
                lexer._register_parser_error_(msg, curr_tok.value);
                set();
            }

            void move_nodes(std::vector<std::unique_ptr<nodes::Node>> &);

            void handle_identifier();

            void parseDataSection();
            void parseTextSection();
            void handleInstruction();

            void handle_proc_declaration();
            void handle_atm_insts();

            void handle_defines(std::string, nodes::NodeKind);
            void handle_definefloats(std::string);
            void handle_string(std::string);

            void handle_resX(std::string);
            void handle_label(std::string);

            void handle_inst_storeX();
            void handle_inst_loadX();
            void handle_inst_store();
            void handle_inst_load();

            void handle_inst_mov();
            void handle_inst_movX();
            void handle_inst_moveX();
            void handle_inst_movsx();

            void handle_inst_Xin(nodes::NodeKind);
            void handle_inst_sin();
            void handle_inst_Xout(nodes::NodeKind);
            void handle_inst_sout();

            void handle_inst_movf();
            void handle_inst_arithmetic();
            void handle_inst_farithmetic();

            void handle_inst_jmpcall(nodes::NodeKind);
            void handle_inst_cmp();
            void handle_inst_jX();
            void handle_inst_incdec(std::string, nodes::NodeKind);
            void handle_inst_logical();
            void handle_inst_shift(std::string, nodes::NodeKind);
            void handle_inst_sva_svc();
            
            void handle_inst_push();
            void handle_inst_pop();
            void handle_inst_lea();
            void handle_inst_excg();
            void handle_inst_excgX();
            void handle_inst_intr();
            void handle_inst_cmpxchg();
        };
    };
};

#endif