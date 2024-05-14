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

#ifndef _ERROR_
#define _ERROR_

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include "../utils/config.hpp"

// During each step, we will print every error encountered
// If a fatal error was discovered, we will not continue to the next step

#define _CCODE_RESET "\033[0m"
#define _CCODE_BLACK "\033[30m"
#define _CCODE_RED "\033[31m"
#define _CCODE_GREEN "\033[32m"
#define _CCODE_YELLOW "\033[33m"
#define _CCODE_BLUE "\033[34m"
#define _CCODE_MAGENTA "\033[35m"
#define _CCODE_CYAN "\033[36m"
#define _CCODE_WHITE "\033[37m"

#define _CCODE_BOLDBLACK "\033[1;30m"
#define _CCODE_BOLDRED "\033[1;31m"
#define _CCODE_BOLDGREEN "\033[1;32m"
#define _CCODE_BOLDYELLOW "\033[1;33m"
#define _CCODE_BOLDBLUE "\033[1;34m"
#define _CCODE_BOLDMAGENTA "\033[1;35m"
#define _CCODE_BOLDCYAN "\033[1;36m"
#define _CCODE_BOLDWHITE "\033[1;37m"

#define _CCODE_UNDERLINE "\033[4m"
#define _CCODE_BOLD "\033[1m"

#define ERROR "ERROR"
#define NOTE "NOTE"

namespace zeta
{
    namespace error
    {

        enum _STATES_
        {
            _NORMAL_,
            _ERR_,  // indicator
            _TRUE_, // indicator

            // for preprocessor
            _FILE_INVALID_,
            _FILE_NOT_AVAI_,
            _FILE_A_DIR_,
            _EXPECTED_PATH_GOT_NL_,

            // for lexer
            _INVALID_TOKEN_,
            _STRING_NOT_TERMINATED_,

            // parse error,
            _PARSE_ERROR_,

        };

        struct Error
        {
            _STATES_ state;              // identify the error, context and the level
            std::string error_msg;       // the error message
            std::string error_line;      // the line where the error occured
            size_t line, col;            // the line and column
            std::string extra_error_msg; // any extra error message[Handled by the error handler]
            std::string error_file;      // the file where the error occured[Handled by the error handler]

            Error(_STATES_ _s, std::string _msg, std::string _l, size_t nl, size_t nc) : state(_s), error_msg(_msg), error_line(_l), line(nl), col(nc) {}
        };

        LOCAL std::vector<std::string> _files_;     // a list or a tree of files that are currently being processed
        LOCAL std::vector<std::string> _inc_files_; // all the included files
        LOCAL std::vector<Error> err;               // all the encountered errors

        // Below here, the first member is "included file" while the second member is "included by"
        // Since there can be multiple "included file"(s) to a single file, we will use that.
        // We will use this to graph a proper include tree; tracing back to the main file
        // This is a very important part for proper error handling
        LOCAL std::unordered_map<std::string, std::string> _inc_tree_; // the inefficient method for error handling

        void append_file(std::string);
        void remove_file();

        bool is_file_already_included(std::string);

        void add_error(Error);

        // check the pool
        // if there are any registered errors, handle them
        // if there are no fatal errors, return
        // else exit without returning
        void _handle_all_errors_();

        std::string _get_base_file_();

        void _update_inc_tree_();

        std::unordered_map<std::string, std::string> _get_inc_tree_();

    };
}

#endif