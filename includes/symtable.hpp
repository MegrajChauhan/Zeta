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

#ifndef _SYMTABLE_
#define _SYMTABLE_

#include <unordered_map>
#include <string>
#include "nodes.hpp"

namespace zeta
{
    namespace symtable
    {
        enum SymEntryType
        {
            _VAR,   // the entry is a variable
            _PROC,  // the entry is a procedure
            _LABEL, // the entry is a label
        };

        // a symtable entry
        struct SymTableEntry
        {
            SymEntryType type;
            std::string value; // if variable, hold the value of the variable
            size_t len = 0, line = 0;
            nodes::DataType dtype; // the data's type[for variables]
            bool defined = false;  // only for some type of entries
            // [in future: for procedures, they could be private to a single file or to multiple file]

            SymTableEntry() = default;

            SymTableEntry(SymEntryType t, std::string val = "", nodes::DataType type = nodes::DataType::_TYPE_BYTE, size_t len = 0, size_t l = 0) : type(t), value(val), dtype(type), len{len}, line(l) {}
        };

        // the symbol table
        class SymTable
        {
            // a variable name or a procedure name will identify the entry
            std::unordered_map<std::string, SymTableEntry> symtable;

        public:
            SymTable() = default;

            void add_entry(std::string, SymTableEntry);
            void add_entry(std::string, SymEntryType, std::string, nodes::DataType);

            std::unordered_map<std::string, SymTableEntry>::iterator find_entry(std::string);

            bool is_valid(std::unordered_map<std::string, SymTableEntry>::iterator);

            auto begin() { return symtable.begin(); }

            auto end() { return symtable.end(); }
        };
    };
};

#endif