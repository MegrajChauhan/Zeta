#ifndef _PREPROCESSOR_
#define _PREPROCESSOR_

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "../utils/config.hpp"
#include "error.hpp"
#include "instructions.hpp"

namespace zeta
{
    namespace prep
    {
        LOCAL std::vector<std::string> included_files;

        class Reader
        {
            std::string _path_to_file_;
            std::vector<std::string> _file_contents_;

        public:
            Reader(std::string);

            // Validate the extension of the given file[Expected ".zasm"]
            _ZETA_ALWAYS_INLINE_ bool is_valid_file();

            // Make sure that the file is indeed a file
            _ZETA_ALWAYS_INLINE_ bool is_file_a_file();

            // Validate the existence of the given file
            _ZETA_ALWAYS_INLINE_ bool validate_existence();

            std::vector<std::string> get_file_contents() { return _file_contents_; }

            bool read();
        };

        class Preprocessor
        {
            std::string inp_file;
            std::vector<std::string> file_contents;
            std::string result; // the result of Preprocessing
            error::_STATES_ state = error::_NORMAL_;

            // For letting others know about errors
            size_t err_line, err_col;
            std::string err_msg, wrong_line;
           
            size_t line = 0;
            size_t col = 0;

        public:
            Preprocessor(std::string);

            auto get_result() { return result; }

            bool process();

            auto get_state() { return state; }
        };
    };
};

#endif