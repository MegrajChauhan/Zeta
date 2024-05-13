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

        public:
            Lexer(std::vector<std::string> &file_conts, std::unordered_map<std::string, prep::_FDetails_> &_d) : file_contents(file_conts), dets(_d)
            {
                iter = file_contents.begin();
                it = iter->begin();
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
        };
    };
};

#endif