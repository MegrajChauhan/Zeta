#ifndef _PREPROCESSOR_
#define _PREPROCESSOR_

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "../utils/config.hpp"
#include "error.hpp"

namespace zeta
{
    namespace prep
    {
        LOCAL std::unordered_map<std::string, std::string> _inc_list_pair;

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
            error::_STATES_ state = _NORMAL_;

        public:
            Preprocessor(std::string);

            auto get_result() { return result; }

            bool process();

            auto get_state() { return state; }
        };
    };
};

#endif