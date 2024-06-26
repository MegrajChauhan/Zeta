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