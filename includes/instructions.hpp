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

#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include "tokens.hpp"
#include <unordered_map>
#include <string>

namespace zeta
{
    using namespace tokens;
    // the global map to identify the token types
    static std::unordered_map<std::string, TokenType> _iden_map_ =
        {
            {"data", _TT_SECTION_DATA},
            {"text", _TT_SECTION_TEXT},
            {"string", _TT_KEY_STRING},
            {"db", _TT_KEY_DB},
            {"dw", _TT_KEY_DW},
            {"dd", _TT_KEY_DD},
            {"dq", _TT_KEY_DQ},
            {"df", _TT_KEY_DF},
            {"dlf", _TT_KEY_DLF},
            {"proc", _TT_KEY_PROC},
            {"atm", _TT_KEY_ATM},
            {"addr", _TT_KEY_ADDR},
            {"const", _TT_KEY_CONST},
            {":", _TT_OPER_COLON},
            {"mov", _TT_INST_MOV},
            {"hlt", _TT_INST_HLT},
            {"movq", _TT_INST_MOVQ},
            {"movb", _TT_INST_MOVB},
            {"movw", _TT_INST_MOVW},
            {"movd", _TT_INST_MOVD},
            {"moveb", _TT_INST_MOVEB},
            {"movew", _TT_INST_MOVEW},
            {"moved", _TT_INST_MOVED},
            {"movf", _TT_INST_MOVF},
            {"movlf", _TT_INST_MOVLF},
            {"movsxb", _TT_INST_MOVSXB},
            {"movsxw", _TT_INST_MOVSXW},
            {"movsxd", _TT_INST_MOVSXD},
            {"nop", _TT_INST_NOP},
            {"outr", _TT_INST_OUTR},
            {"uoutr", _TT_INST_UOUTR},
            {"cin", _TT_INST_CIN},
            {"cout", _TT_INST_COUT},
            {"resb", _TT_KEY_RESB},
            {"resw", _TT_KEY_RESW},
            {"resd", _TT_KEY_RESD},
            {"resq", _TT_KEY_RESQ},
            {"sin", _TT_INST_SIN},
            {"sout", _TT_INST_SOUT},
            {"in", _TT_INST_IN},
            {"out", _TT_INST_OUT},
            {"inw", _TT_INST_INW},
            {"outw", _TT_INST_OUTW},
            {"ind", _TT_INST_IND},
            {"outd", _TT_INST_OUTD},
            {"inq", _TT_INST_INQ},
            {"outq", _TT_INST_OUTQ},
            {"uin", _TT_INST_UIN},
            {"uout", _TT_INST_UOUT},
            {"uinw", _TT_INST_UINW},
            {"uoutw", _TT_INST_UOUTW},
            {"uind", _TT_INST_UIND},
            {"uoutd", _TT_INST_UOUTD},
            {"uinq", _TT_INST_UINQ},
            {"uoutq", _TT_INST_UOUTQ},
            {"outf", _TT_INST_OUTF},
            {"outlf", _TT_INST_OUTLF},
            {"inf", _TT_INST_INF},
            {"inlf", _TT_INST_INLF},
            {"add", _TT_INST_ADD},
            {"sub", _TT_INST_SUB},
            {"mul", _TT_INST_MUL},
            {"div", _TT_INST_DIV},
            {"mod", _TT_INST_MOD},
            {"iadd", _TT_INST_IADD},
            {"isub", _TT_INST_ISUB},
            {"imul", _TT_INST_IMUL},
            {"idiv", _TT_INST_IDIV},
            {"imod", _TT_INST_IMOD},
            {"fadd", _TT_INST_FADD},
            {"lfadd", _TT_INST_LFADD},
            {"fsub", _TT_INST_FSUB},
            {"lfsub", _TT_INST_LFSUB},
            {"fmul", _TT_INST_FMUL},
            {"lfmul", _TT_INST_LFMUL},
            {"fdiv", _TT_INST_FDIV},
            {"lfdiv", _TT_INST_LFDIV},
            {"jmp", _TT_INST_JMP},
            {"cmp", _TT_INST_CMP},
            {"jnz", _TT_INST_JNZ},
            {"jz", _TT_INST_JZ},
            {"jne", _TT_INST_JNE},
            {"je", _TT_INST_JE},
            {"jnc", _TT_INST_JNC},
            {"jc", _TT_INST_JC},
            {"jno", _TT_INST_JNO},
            {"jo", _TT_INST_JO},
            {"jnn", _TT_INST_JNN},
            {"jn", _TT_INST_JN},
            {"jng", _TT_INST_JNG},
            {"jg", _TT_INST_JG},
            {"jns", _TT_INST_JNS},
            {"js", _TT_INST_JS},
            {"jge", _TT_INST_JGE},
            {"jse", _TT_INST_JSE},
            {"and", _TT_INST_AND},
            {"or", _TT_INST_OR},
            {"xor", _TT_INST_XOR},
            {"not", _TT_INST_NOT},
            {"lshift", _TT_INST_LSHIFT},
            {"rshift", _TT_INST_RSHIFT},
            {"inc", _TT_INST_INC},
            {"dec", _TT_INST_DEC},
            {"cflags", _TT_INST_CFLAGS},
            {"reset", _TT_INST_RESET},
            {"clz", _TT_INST_CLZ},
            {"cln", _TT_INST_CLN},
            {"clc", _TT_INST_CLC},
            {"clo", _TT_INST_CLO},
            {"call", _TT_INST_CALL},
            {"ret", _TT_INST_RET},
            {"sva", _TT_INST_SVA},
            {"svc", _TT_INST_SVC},
            {"push", _TT_INST_PUSH},
            {"pusha", _TT_INST_PUSHA},
            {"pop", _TT_INST_POP},
            {"popa", _TT_INST_POPA},
            {"lea", _TT_INST_LEA},
            {"store", _TT_INST_STORE},
            {"load", _TT_INST_LOAD},
            {"excg", _TT_INST_EXCG},
            {"excgb", _TT_INST_EXCG8},
            {"excgw", _TT_INST_EXCG16},
            {"excgd", _TT_INST_EXCG32},
            {"loop", _TT_INST_LOOP},
            {"int", _TT_INST_INTR},
            {"storeb", _TT_INST_STOREB},
            {"loadb", _TT_INST_LOADB},
            {"storew", _TT_INST_STOREW},
            {"stored", _TT_INST_STORED},
            {"loadd", _TT_INST_LOADD},
            {"cmpxchg", _TT_INST_CMPXCHG},
            {"ecall", _TT_INST_CALL_EXCP},
            {"set", _TT_INST_SET_EXCP},
    };
};

#endif