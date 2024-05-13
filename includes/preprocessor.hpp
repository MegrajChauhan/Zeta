#ifndef _PREPROCESSOR_
#define _PREPROCESSOR_

/*
  It is not really preprocessing.
  We won't have anything except "use" which will include files and that is all
*/

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "error.hpp"

namespace zeta
{
    namespace prep
    {
        struct _FDetails_
        {
            size_t st, end;
        };

        class Preprocessor
        {
            // processing using vectors of strings makes it a lot easier
            std::vector<std::string> file_contents;
            std::unordered_map<std::string, _FDetails_> fdetails;
            size_t refr_line = 0;
            size_t line = 0, col = 0;

        public:
            Preprocessor() = default;

            zeta::error::_STATES_ prepare(std::string);

            zeta::error::_STATES_ process();

            auto get_final_result() { return file_contents; }

            auto get_dets() { return fdetails; }
        };
    };
};

#endif