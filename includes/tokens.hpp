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

            _TT_INST_INC,
            _TT_INST_DEC,
            // we ignore commas, they are not absolutely necessary and the assembler won't even complain
            // about not using it. They are just there to improve readability
        };
    };
};

#endif