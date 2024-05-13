// Testing the preprocessor

#include <iostream>
#include "includes/preprocessor.hpp"
#include "includes/lexer.hpp"

int main()
{
    std::string inp = "common.zasm";
    using namespace zeta;
    prep::Preprocessor p;
    auto res = p.prepare(inp);
    if (res != error::_NORMAL_)
    {
        switch (res)
        {
        case error::_FILE_INVALID_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << inp << _CCODE_RESET << " is an invalid input file. Expected it to end with \".zasm\"\n";
            break;
        case error::_FILE_NOT_AVAI_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << inp << _CCODE_RESET << " doesn't exist.\n";
            break;
        case error::_FILE_A_DIR_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << inp << _CCODE_RESET << " is a directory.\n";
            break;
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        p.process(); // return value doesn't really matter right now
    }
    error::_handle_all_errors_();
    auto r = p.get_final_result();
    auto dets = p.get_dets();
    lexer::Lexer l(r, dets);
    auto tok = l.next_token();
    while (tok.type != tokens::_TT_EOF)
    {
        std::cout << tok.type << " " << tok.value << "\n";
        tok = l.next_token();
    }
}