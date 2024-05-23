// Testing the preprocessor

#include <iostream>
#include "includes/preprocessor.hpp"
#include "includes/lexer.hpp"
#include "includes/parser.hpp"
#include "includes/sema.hpp"
#include "codegen/codegen.hpp"
#include "codegen/emit.hpp"
#include "includes/asm.hpp"

using namespace zeta;

#define VERSION "Masm- v0.0.0-alpha.1c"

static std::string version_message = "Zeta: An Assembler for the Merry Virtual Machine.\n"
                                     "Latest version: ";

int main(int argv, char **argc)
{
    zeta::Masm _asm(argv, argc);
    std::pair<bool, std::string> input_filename, output_filename;

    _asm.parse_args();
    if (_asm.get_option("help").first)
    {
        _asm.print_help();
        return 0;
    }
    else if (_asm.get_option("version").first)
    {
        std::cout << version_message + VERSION << std::endl;
        return 0;
    }
    if (!_asm.get_option("input").first)
    {
        std::cerr << "Error: No input file provided\n";
        _asm.print_help();
        return -1;
    }
    output_filename = _asm.get_option("output");
    input_filename = _asm.get_option("input");

    prep::Preprocessor _p;
    auto res = _p.prepare(input_filename.second);
    if (res != error::_NORMAL_)
    {
        switch (res)
        {
        case error::_FILE_INVALID_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << input_filename.second << _CCODE_RESET << " is an invalid input file. Expected it to end with \".zasm\"\n";
            break;
        case error::_FILE_NOT_AVAI_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << input_filename.second << _CCODE_RESET << " doesn't exist.\n";
            break;
        case error::_FILE_A_DIR_:
            std::cerr << "The given input file " << _CCODE_BOLDWHITE << input_filename.second << _CCODE_RESET << " is a directory.\n";
            break;
        }
        exit(EXIT_FAILURE);
    }
    res = _p.process();
    if (res != error::_NORMAL_)
        error::_handle_all_errors_();
    // if we are here that means there were no preprocessing errors
    auto file_conts = _p.get_final_result();
    auto dets = _p.get_dets();

    lexer::Lexer _l(file_conts, dets);
    parser::Parser parser(_l);
    if (parser.parse() != error::_NORMAL_)
        error::_handle_all_errors_();
    // We are here i.e no errors
    zeta::sema::Sema sema(_l, parser);
    if (sema.analyse() != error::_NORMAL_)
        error::_handle_all_errors_();
    zeta::codegen::Codegen codegen(sema);
    zeta::emit::Emit emit(codegen);
    emit.set_output_filename(output_filename.first ? output_filename.second : "");
    emit.emit();
}