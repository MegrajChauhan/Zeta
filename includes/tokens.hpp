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

#ifndef _TOKENS_
#define _TOKENS_

namespace zeta
{
    namespace tokens
    {
        enum TokenType
        {
            _TT_ERR, // return for functions
            _TT_EOF,
            _TT_SECTION_DATA,
            _TT_SECTION_TEXT,
            _TT_KEY_STRING, // the ".string" keyword
            _TT_KEY_DB,     // "db" or define byte
            _TT_KEY_DW,
            _TT_KEY_DD,
            _TT_KEY_DQ,
            _TT_KEY_DF,
            _TT_KEY_DLF,
            _TT_KEY_RESB,
            _TT_KEY_RESW,
            _TT_KEY_RESD,
            _TT_KEY_RESQ,
            _TT_KEY_PROC,
            _TT_KEY_ATM,

            _TT_STRING,
            _TT_INT,
            _TT_NINT,
            _TT_FLOAT,
            _TT_NFLOAT,

            _TT_OPER_COLON,

            _TT_IDENTIFIER,

            // for the lexer, even the register names like Ma, Mb are IDENTIFIER, it's interpretation is upto the parser
            _TT_INST_NOP,
            _TT_INST_MOV,
            _TT_INST_HLT,

            _TT_INST_MOVQ,

            _TT_INST_MOVB,
            _TT_INST_MOVW,
            _TT_INST_MOVD,

            _TT_INST_MOVEB,
            _TT_INST_MOVEW,
            _TT_INST_MOVED,

            _TT_INST_MOVF,
            _TT_INST_MOVLF,

            _TT_INST_MOVSXB,
            _TT_INST_MOVSXW,
            _TT_INST_MOVSXD,

            _TT_INST_OUTR,
            _TT_INST_UOUTR,

            _TT_INST_CIN,
            _TT_INST_COUT,
            _TT_INST_SIN,
            _TT_INST_SOUT,
            _TT_INST_IN,
            _TT_INST_OUT,
            _TT_INST_INW,
            _TT_INST_OUTW,
            _TT_INST_IND,
            _TT_INST_OUTD,
            _TT_INST_INQ,
            _TT_INST_OUTQ,

            _TT_INST_UIN,
            _TT_INST_UOUT,
            _TT_INST_UINW,
            _TT_INST_UOUTW,
            _TT_INST_UIND,
            _TT_INST_UOUTD,
            _TT_INST_UINQ,
            _TT_INST_UOUTQ,

            _TT_INST_OUTF,
            _TT_INST_OUTLF,
            _TT_INST_INF,
            _TT_INST_INLF,

            _TT_INST_ADD,
            _TT_INST_SUB,
            _TT_INST_MUL,
            _TT_INST_DIV,
            _TT_INST_MOD,

            _TT_INST_IADD,
            _TT_INST_ISUB,
            _TT_INST_IMUL,
            _TT_INST_IDIV,
            _TT_INST_IMOD,

            _TT_INST_FADD,
            _TT_INST_FSUB,
            _TT_INST_FMUL,
            _TT_INST_FDIV,

            _TT_INST_LFADD,
            _TT_INST_LFSUB,
            _TT_INST_LFMUL,
            _TT_INST_LFDIV,

            _TT_INST_JMP,
            _TT_INST_CMP,

            // Implement these next
            _TT_INST_JNZ,
            _TT_INST_JZ,
            _TT_INST_JNE,
            _TT_INST_JE,
            _TT_INST_JNC,
            _TT_INST_JC,
            _TT_INST_JNO,
            _TT_INST_JO,
            _TT_INST_JNN,
            _TT_INST_JN,
            _TT_INST_JNG,
            _TT_INST_JG,
            _TT_INST_JNS,
            _TT_INST_JS,
            _TT_INST_JGE,
            _TT_INST_JSE,

            _TT_INST_AND,
            _TT_INST_OR,
            _TT_INST_XOR,
            _TT_INST_NOT,
            _TT_INST_LSHIFT,
            _TT_INST_RSHIFT,

            _TT_INST_CFLAGS,
            _TT_INST_RESET,
            _TT_INST_CLZ,
            _TT_INST_CLN,
            _TT_INST_CLC,
            _TT_INST_CLO,

            _TT_INST_CALL,
            _TT_INST_RET,

            _TT_INST_SVA,
            _TT_INST_SVC,

            _TT_INST_PUSH,
            _TT_INST_POP,
            _TT_INST_PUSHA,
            _TT_INST_POPA,

            _TT_INST_LEA,
            _TT_INST_LOAD,
            _TT_INST_STORE,
            _TT_INST_EXCG8,
            _TT_INST_EXCG16,
            _TT_INST_EXCG32,
            _TT_INST_EXCG,
            _TT_INST_LOOP,
            _TT_INST_INTR,
            _TT_INST_LOADB,
            _TT_INST_STOREB,
            _TT_INST_LOADW,
            _TT_INST_STOREW,
            _TT_INST_LOADD,
            _TT_INST_STORED,
            _TT_INST_CMPXCHG,

            _TT_INST_SET_EXCP,
            _TT_INST_CALL_EXCP,

            _TT_INST_INC,
            _TT_INST_DEC,
            // we ignore commas, they are not absolutely necessary and the assembler won't even complain
            // about not using it. They are just there to improve readability
        };
    };

    namespace nodes
    {
                enum NodeType
        {
            _TYPE_DATA,
            _TYPE_INST,
        };

        enum DataType
        {
            _TYPE_BYTE,
            _TYPE_WORD,
            _TYPE_DWORD,
            _TYPE_QWORD,
            _TYPE_STRING,
            _TYPE_RESB,
            _TYPE_RESW,
            _TYPE_RESD,
            _TYPE_RESQ,
            _TYPE_FLOAT,
            _TYPE_LFLOAT,
        };

        enum NodeKind
        {
            // the kind of node(Not Type but like move, define byte, string etc)
            _DEF_BYTE, // defines a byte
            _DEF_WORD,
            _DEF_DWORD,
            _DEF_QWORD,
            _DEF_STRING,
            _DEF_RESB,
            _DEF_RESW,
            _DEF_RESD,
            _DEF_RESQ,
            _DEF_FLOAT,
            _DEF_LFLOAT,
            // any label that is declared as proc is a procedure while any other are just labels
            _PROC_DECLR, // procedure declaration
            _LABEL,      // A label[Unless semantically verified even procedure definition is a label]
            _INST_NOP,
            _INST_MOV_REG_IMM,
            _INST_MOV_REG_REG,
            _INST_MOV_REG_IMMQ,
            _INST_MOV_REG_MOVEB,
            _INST_MOV_REG_MOVEW,
            _INST_MOV_REG_MOVED,
            _INST_MOV_REG_REG8,
            _INST_MOV_REG_REG16,
            _INST_MOV_REG_REG32,
            _INST_MOV_REG_IMM8,
            _INST_MOV_REG_IMM16,
            _INST_MOV_REG_IMM32,
            _INST_MOVF,
            _INST_MOVLF,

            _INST_MOVSX_REG_REG8,
            _INST_MOVSX_REG_REG16,
            _INST_MOVSX_REG_REG32,
            _INST_MOVSX_REG_IMM8,
            _INST_MOVSX_REG_IMM16,
            _INST_MOVSX_REG_IMM32,

            _INST_OUTR,
            _INST_UOUTR,

            _INST_CIN,
            _INST_SIN,
            _INST_COUT,
            _INST_SOUT,
            _INST_IN,
            _INST_OUT,
            _INST_INW,
            _INST_OUTW,
            _INST_IND,
            _INST_OUTD,
            _INST_INQ,
            _INST_OUTQ,
            _INST_UIN,
            _INST_UOUT,
            _INST_UINW,
            _INST_UOUTW,
            _INST_UIND,
            _INST_UOUTD,
            _INST_UINQ,
            _INST_UOUTQ,
            _INST_INF,
            _INST_OUTF,
            _INST_INLF,
            _INST_OUTLF,

            _INST_ADD_IMM,
            _INST_ADD_REG,
            _INST_IADD_IMM,
            _INST_IADD_REG,

            _INST_SUB_IMM,
            _INST_SUB_REG,
            _INST_ISUB_IMM,
            _INST_ISUB_REG,

            _INST_MUL_IMM,
            _INST_MUL_REG,
            _INST_IMUL_IMM,
            _INST_IMUL_REG,

            _INST_DIV_IMM,
            _INST_DIV_REG,
            _INST_IDIV_IMM,
            _INST_IDIV_REG,

            _INST_MOD_IMM,
            _INST_MOD_REG,
            _INST_IMOD_IMM,
            _INST_IMOD_REG,

            _INST_FADD_REG,
            _INST_FSUB_REG,
            _INST_FMUL_REG,
            _INST_FDIV_REG,
            _INST_FADD_IMM,
            _INST_FSUB_IMM,
            _INST_FMUL_IMM,
            _INST_FDIV_IMM,

            _INST_LFADD_REG,
            _INST_LFSUB_REG,
            _INST_LFMUL_REG,
            _INST_LFDIV_REG,
            _INST_LFADD_IMM,
            _INST_LFSUB_IMM,
            _INST_LFMUL_IMM,
            _INST_LFDIV_IMM,

            // even though we have two kinds of jumps, the jmp_off won't be used
            // instead the jmp_addr variant will be used
            _INST_JMP,
            _INST_CMP_REG, // comparing two registers
            _INST_CMP_IMM, // comparing with immediates

            _INST_JNZ,
            _INST_JZ,
            _INST_JNE,
            _INST_JE,
            _INST_JNC,
            _INST_JC,
            _INST_JNO,
            _INST_JO,
            _INST_JNN,
            _INST_JN,
            _INST_JNG,
            _INST_JG,
            _INST_JNS,
            _INST_JS,
            _INST_JGE,
            _INST_JSE,

            _INST_INC,
            _INST_DEC,

            _INST_AND_IMM,
            _INST_AND_REG,
            _INST_OR_IMM,
            _INST_OR_REG,
            _INST_XOR_IMM,
            _INST_XOR_REG,
            _INST_NOT,

            _INST_LSHIFT,
            _INST_RSHIFT,

            _INST_CFLAGS,
            _INST_RESET,
            _INST_CLZ,
            _INST_CLN,
            _INST_CLC,
            _INST_CLO,
            _INST_RET,
            _INST_CALL,
            _INST_SVA,
            _INST_SVC,
            _INST_PUSH_IMM,
            _INST_PUSH_REG,
            _INST_PUSHA,
            _INST_POP,
            _INST_POPA,

            _INST_LEA,
            
            _INST_STORE,
            _INST_LOAD,
            _INST_STOREB,
            _INST_LOADB,
            _INST_STOREW,
            _INST_LOADW,
            _INST_STORED,
            _INST_LOADD,

            _INST_STORE_REG,
            _INST_LOAD_REG,
            _INST_STOREB_REG,
            _INST_LOADB_REG,
            _INST_STOREW_REG,
            _INST_LOADW_REG,
            _INST_STORED_REG,
            _INST_LOADD_REG,
            
            _INST_ATM_STORE,
            _INST_ATM_LOAD,
            _INST_ATM_STOREB,
            _INST_ATM_LOADB,
            _INST_ATM_STOREW,
            _INST_ATM_LOADW,
            _INST_ATM_STORED,
            _INST_ATM_LOADD,
            
            _INST_EXCG,
            _INST_EXCG8,
            _INST_EXCG16,
            _INST_EXCG32,
            _INST_LOOP,
            _INST_INTR,
            _INST_CMPXCHG,
            
            _INST_SET_EXCP,
            _INST_CALL_EXCP,

            _INST_HLT, // this doesn't need its own structure
        };
    };
};

#endif
