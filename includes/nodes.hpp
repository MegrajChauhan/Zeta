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

#ifndef _NODES_
#define _NODES_

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "tokens.hpp"

namespace zeta
{
    namespace nodes
    {
        enum Registers
        {
            Ma,
            Mb,
            Mc,
            Md,
            Me,
            Mf,
            M1,
            M2,
            M3,
            M4,
            M5,
            Mm1,
            Mm2,
            Mm3,
            Mm4,
            Mm5,
        };

        static std::unordered_map<std::string, Registers> _regr_iden_map = {
            {"Ma", Ma},
            {"Mb", Mb},
            {"Mc", Mc},
            {"Md", Md},
            {"Me", Me},
            {"Mf", Mf},
            {"M1", M1},
            {"M2", M2},
            {"M3", M3},
            {"M4", M4},
            {"M5", M5},
            {"Mm1", Mm1},
            {"Mm2", Mm2},
            {"Mm3", Mm3},
            {"Mm4", Mm4},
            {"Mm5", Mm5},
        };

        // Define base class
        struct Base
        {
            virtual ~Base() {} // Make the base class polymorphic with a virtual destructor
        };

        struct NodeCmpRegr : public Base
        {
            Registers regr1; // the first operand register
            Registers regr2; // the second operand register
        };

        struct NodeExcg : public Base
        {
            Registers regr1;
            Registers regr2;
        };

        struct NodeCmpXchg : public Base
        {
            Registers r1, r2;
            std::string var_name;
        };

        struct NodeCmpImm : public Base
        {
            Registers regr; // the register operand
            std::string val;
            bool is_iden = false; // if set, val will be treated as a variable name
        };

        // we need this so
        struct NodeNop : public Base
        {
        };

        // Define derived classes for each node type
        struct NodeDefs : public Base
        {
            std::string byte_val;  
            std::string byte_name;
        };

        struct NodeLea : public Base
        {
            Registers dest, base, index, scale;
        };

        struct NodeControlFlow : public Base
        {
            std::string _jmp_label_; // the label to jump to
        };

        struct NodeProcDeclr : public Base
        {
            std::string proc_name; // the proc's name
        };

        struct NodeLabel : public Base
        {
            std::string label_name;
        };

        struct NodeInstMovRegImm : public Base
        {
            bool is_iden = false; // interpret value as a variable name
            Registers dest_regr;  // destination register
            std::string value;    // the value of to move
            // the data type is most likely INT or NUM
        };

        struct NodeInstMovRegReg : public Base
        {
            Registers dest_regr; // destination register
            Registers src_reg;   // the source register
        };

        struct NodeStore : public Base
        {
            Registers dest;
            std::string var_name;
        };

        struct NodeStoreReg : public Base
        {
            Registers dest, addr_regr;
        };

        struct NodeLoad : public Base
        {
            Registers dest;
            std::string var_name;
        };

        struct NodeLoadReg : public Base
        {
            Registers dest, addr_regr;
        };


        struct NodeLogicalRegImm : public Base
        {
            Registers dest_regr;
            std::string value;
        };

        struct NodeLogicalRegReg : public Base
        {
            Registers dest_regr;
            Registers src_reg;
        };

        struct NodeShifts : public Base
        {
            Registers dest_regr;
            std::string value;
        };

        // this struct is for any instruction that uses just one register operand
        struct NodeOneRegrOperands : public Base
        {
            Registers oper_rger;
        };

        struct NodeOneImmOperand : public Base
        {
            std::string imm;
        };

        struct NodeRes : public Base
        {
            std::string name;
            size_t number;
        };

        struct NodeArithmeticRegReg : public Base
        {
            Registers dest_regr;
            Registers src_reg;
        };

        struct NodeArithmeticRegImm : public Base
        {
            bool is_iden = false;
            Registers dest_regr;
            std::string value;
        };

        // Define struct Node to hold a pointer to Base
        struct Node
        {
            NodeType type; // where this node is from
            NodeKind kind;
            std::unique_ptr<Base> ptr;
            size_t line;

            Node() {}

            Node(NodeType t, NodeKind k, std::unique_ptr<Base> p, size_t pos = 0) : type(t), kind(k), ptr(std::move(p)), line(pos) {}

            // Copy constructor
            Node(const Node &node) : type(node.type), kind(node.kind), line(node.line)
            {
                if (node.ptr)
                {
                    ptr = std::make_unique<Base>(*node.ptr);
                }
            }

            // Assignment operator
            Node &operator=(const Node &node)
            {
                if (this != &node)
                {
                    type = node.type;
                    kind = node.kind;
                    line = node.line;
                    if (node.ptr)
                    {
                        ptr = std::make_unique<Base>(*node.ptr);
                    }
                    else
                    {
                        ptr.reset();
                    }
                }
                return *this;
            }
        };
    };
};

#endif