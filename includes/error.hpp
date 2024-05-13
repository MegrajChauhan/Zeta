#ifndef _ERROR_
#define _ERROR_

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "../utils/config.hpp"

// During each step, we will print every error encountered
// If a fatal error was discovered, we will not continue to the next step

namespace zeta
{
    namespace error
    {
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

        enum _STATES_
        {
            _NORMAL_,
            _ERR_,  // indicator
            _TRUE_, // indicator

            // for preprocessor
            _READ_FAILED_,
            _FILE_INVALID_,
            _FILE_NOT_AVAI_,
            _FILE_A_DIR_,
            _PREPROCESSING_ERR_, // contains program error
            _EXPECTED_PATH_GOT_NL_,

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

        void append_file(std::string);
        void remove_file();

        bool is_file_already_included(std::string);

        void add_error(Error);

        // check the pool
        // if there are any registered errors, handle them
        // if there are no fatal errors, return
        // else exit without returning
        void _handle_all_errors_();

    };
}

#endif