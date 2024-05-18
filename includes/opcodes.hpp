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

#ifndef _OPCODES_
#define _OPCODES_

// defines the opcodes

/*
 Copied exactly same from merry's internals
 All the explainaitions for every instruction can be found in docs/opcodes.txt
*/

/*
  NOTE:
  If your VM follows the same rule for instructions as Merry the change the following
  for the getting correct output in your VM.
*/
namespace zeta
{
    namespace opcodes
    {
        enum opcodes
        {
            // ------------------------ DONE
            OP_NOP,
            OP_HALT,
            OP_ADD_IMM,
            OP_ADD_REG,
            OP_SUB_IMM,
            OP_SUB_REG,
            OP_MUL_IMM,
            OP_MUL_REG,
            OP_DIV_IMM,
            OP_DIV_REG,
            OP_MOD_IMM,
            OP_MOD_REG,
            OP_IADD_IMM,
            OP_IADD_REG,
            OP_ISUB_IMM,
            OP_ISUB_REG,
            OP_IMUL_IMM,
            OP_IMUL_REG,
            OP_IDIV_IMM,
            OP_IDIV_REG,
            OP_IMOD_IMM,
            OP_IMOD_REG,
            OP_FADD,
            OP_FSUB,
            OP_FMUL,
            OP_FDIV,
            OP_MOVE_IMM,
            OP_MOVE_IMM_64,
            OP_MOVE_REG,
            OP_MOVE_REG8,
            OP_MOVE_REG16,
            OP_MOVE_REG32,
            OP_MOVESX_IMM8,
            OP_MOVESX_IMM16,
            OP_MOVESX_IMM32,
            OP_MOVESX_REG8,
            OP_MOVESX_REG16,
            OP_MOVESX_REG32,
            OP_JMP_OFF,
            OP_JMP_ADDR,
            OP_CALL,
            OP_RET,
            OP_SVA,
            OP_SVC,
            OP_PUSH_IMM,
            OP_PUSH_REG,
            OP_POP,
            OP_PUSHA,
            OP_POPA,
            OP_AND_IMM,
            OP_AND_REG,
            OP_OR_IMM,
            OP_OR_REG,
            OP_XOR_IMM,
            OP_XOR_REG,
            OP_NOT,
            OP_LSHIFT,
            OP_RSHIFT,
            OP_CMP_IMM,
            OP_CMP_REG,
            OP_INC,
            OP_DEC,
            OP_LEA,
            OP_LOAD,
            OP_STORE,
            OP_EXCG8,
            OP_EXCG16,
            OP_EXCG32,
            OP_EXCG,
            OP_MOV8,
            OP_MOV16,
            OP_MOV32,
            OP_CFLAGS,
            OP_RESET,
            OP_CLZ,
            OP_CLN,
            OP_CLC,
            OP_CLO,
            OP_JNZ,
            OP_JZ,
            OP_JNE,
            OP_JE,
            OP_JNC,
            OP_JC,
            OP_JNO,
            OP_JO,
            OP_JNN,
            OP_JN,
            OP_JNG,
            OP_JG,
            OP_JNS,
            OP_JS,
            OP_JGE,
            OP_JSE,
            OP_LOOP,
            OP_INTR,
            OP_LOADB,
            OP_LOADW,
            OP_LOADD,
            OP_STOREB,
            OP_STOREW,
            OP_STORED,
            OP_LOAD_REG,
            OP_STORE_REG,
            OP_LOADB_REG,
            OP_STOREB_REG,
            OP_LOADW_REG,
            OP_STOREW_REG,
            OP_LOADD_REG,
            OP_STORED_REG,
            OP_CMPXCHG,
            OP_CIN,
            OP_COUT,
            OP_SIN,
            OP_SOUT,
            OP_IN,
            OP_OUT,
            OP_INW,
            OP_OUTW,
            OP_IND,
            OP_OUTD,
            OP_INQ,
            OP_OUTQ,
            OP_UIN,
            OP_UOUT,
            OP_UINW,
            OP_UOUTW,
            OP_UIND,
            OP_UOUTD,
            OP_UINQ,
            OP_UOUTQ,
            OP_INF,
            OP_OUTF,
            OP_INF32,
            OP_OUTF32,
            OP_OUTR,
            OP_UOUTR,
            OP_FADD32,
            OP_FSUB32,
            OP_FMUL32,
            OP_FDIV32,
            // ------------------------ DONE

            OP_ATOMIC_LOAD,
            OP_ATOMIC_LOADB,
            OP_ATOMIC_LOADW,
            OP_ATOMIC_LOADD,
            OP_ATOMIC_STORE,
            OP_ATOMIC_STOREB,
            OP_ATOMIC_STOREW,
            OP_ATOMIC_STORED,

            // ------------------------ DONE
            OP_ADD_MEMB,
            OP_ADD_MEMW,
            OP_ADD_MEMD,
            OP_ADD_MEMQ,
            OP_SUB_MEMB,
            OP_SUB_MEMW,
            OP_SUB_MEMD,
            OP_SUB_MEMQ,
            OP_MUL_MEMB,
            OP_MUL_MEMW,
            OP_MUL_MEMD,
            OP_MUL_MEMQ,
            OP_DIV_MEMB,
            OP_DIV_MEMW,
            OP_DIV_MEMD,
            OP_DIV_MEMQ,
            OP_MOD_MEMB,
            OP_MOD_MEMW,
            OP_MOD_MEMD,
            OP_MOD_MEMQ,
            OP_FADD_MEM,
            OP_FSUB_MEM,
            OP_FMUL_MEM,
            OP_FDIV_MEM,
            OP_FADD32_MEM,
            OP_FSUB32_MEM,
            OP_FMUL32_MEM,
            OP_FDIV32_MEM,
            OP_CMP_IMM_MEMB,
            OP_CMP_IMM_MEMW,
            OP_CMP_IMM_MEMD,
            OP_CMP_IMM_MEMQ,
            // ------------------------ DONE

            OP_SET_EXCP,
            OP_CALL_EXCP,
        };
    };
};

#endif
