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

#ifndef _LEXER_
#define _LEXER_

#include "error.hpp"
#include <string>
#include <vector>
#include <stack>
#include "tokens.hpp"
#include "instructions.hpp"
#include "preprocessor.hpp"

#define should_skip(ch) (ch == ',' || ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\0') // add more as needed
#define is_num(ch) (ch >= '0' && ch <= '9')
#define is_alpha(ch) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
#define is_oper(ch) (ch == ':')

namespace zeta
{
    namespace lexer
    {
        struct Token
        {
            tokens::TokenType type; // the token's type
            std::string value;      // for eg the token _TT_INT could have a value "123" from the code

            Token() = default;

            Token(tokens::TokenType _t, std::string _v = " ") : type(_t), value(_v) {}
        };

        class Lexer
        {
            std::vector<std::string> &file_contents;
            size_t line = 0, col = 0;
            std::vector<std::string>::iterator iter;
            std::string::iterator it;
            std::unordered_map<std::string, prep::_FDetails_> &dets;
            std::unordered_map<std::string, std::string> inc_tree;
            std::string _base_file_name_;

        public:
            Lexer(std::vector<std::string> &file_conts, std::unordered_map<std::string, prep::_FDetails_> &_d) : file_contents(file_conts), dets(_d)
            {
                iter = file_contents.begin();
                it = iter->begin();
                inc_tree = error::_get_inc_tree_();
                _base_file_name_ = error::_get_base_file_();
            }

            void consume();

            void consume_comments();

            void clear_unnecessary();

            char peek();

            Token get_iden_or_keyword();

            Token get_number();

            std::string get_current_line();

            std::string get_string();

            Token next_token();

            std::pair<std::string, prep::_FDetails_> get_file(); // get the file name based on the current line number

            void _add_error_(error::_STATES_, std::string);

            size_t get_base_file_line_no();

            void _register_parser_error_(std::string, std::string);

            void _register_sema_error_(size_t, std::string, error::_STATES_);

            size_t get_current_line_no() { return line; }
        };
    };
};

#endif