#include "../codegen/codegen.hpp"

zeta::codegen::Codegen::Codegen(std::vector<std::unique_ptr<zeta::nodes::Node>> &src)
{
    inst_nodes = std::move(src);
}

zeta::codegen::Codegen::Codegen(zeta::sema::Sema &sema)
{
    sema.pass_nodes(inst_nodes);
    table = sema.get_symtable();
    main_proc_ind = sema.get_main_declr_pos();
}

void zeta::codegen::Codegen::gen_data()
{
    // we need to generate addresses for all the data
    // and based on their sizes as well
    // data_addrs will hold the addresses to the first bytes of the data
    size_t start = 0; // start with addr 0
    for (auto data : table)
    {
        switch (data.second.type)
        {
        case symtable::SymEntryType::_VAR:
        {
            // don't care about procedures and labels here
            switch (data.second.dtype)
            {
            case nodes::DataType::_TYPE_RESB:
            {
                data_addrs[data.first] = start;
                start += data.second.len; // just 1 byte but for alignment issues recently encountered, we will have to do this
                unsigned char x = 0;
                data_bytes.insert(data_bytes.end(), data.second.len, x);
                break;
            }
            case nodes::DataType::_TYPE_RESW:
            {
                data_addrs[data.first] = start;
                start += data.second.len * 2; // just 1 byte but for alignment issues recently encountered, we will have to do this
                unsigned char x = 0;
                data_bytes.insert(data_bytes.end(), data.second.len * 2, x);
                break;
            }
            case nodes::DataType::_TYPE_RESD:
            {
                data_addrs[data.first] = start;
                start += data.second.len * 4; // just 1 byte but for alignment issues recently encountered, we will have to do this
                unsigned char x = 0;
                data_bytes.insert(data_bytes.end(), data.second.len * 4, x);
                break;
            }
            case nodes::DataType::_TYPE_RESQ:
            {
                data_addrs[data.first] = start;
                start += data.second.len * 8; // just 1 byte but for alignment issues recently encountered, we will have to do this
                unsigned char x = 0;
                data_bytes.insert(data_bytes.end(), data.second.len * 8, x);
                break;
            }
            case nodes::DataType::_TYPE_BYTE:
            {
                // since the variable names are unique, we won't have to worry about anything here
                data_addrs[data.first] = start;
                start += 2; // just 1 byte but for alignment issues recently encountered, we will have to do this
                data_bytes.push_back(std::stoi(data.second.value) & 0xFF);
                data_bytes.push_back(0);
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                // since the variable names are unique, we won't have to worry about anything here
                data_addrs[data.first] = start;
                start += 2;
                uint16_t val = std::stoi(data.second.value);
                data_bytes.push_back(val & 255);
                data_bytes.push_back((val >> 8) & 255);
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                // since the variable names are unique, we won't have to worry about anything here
                data_addrs[data.first] = start;
                start += 4;
                uint32_t val = std::stoi(data.second.value);
                data_bytes.push_back(val & 255);
                data_bytes.push_back((val >> 8) & 255);
                data_bytes.push_back((val >> 16) & 255);
                data_bytes.push_back((val >> 24) & 255);
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                // since the variable names are unique, we won't have to worry about anything here
                data_addrs[data.first] = start;
                start += 8;
                uint64_t val = std::stoull(data.second.value);
                data_bytes.push_back(val & 255);
                data_bytes.push_back((val >> 8) & 255);
                data_bytes.push_back((val >> 16) & 255);
                data_bytes.push_back((val >> 24) & 255);
                data_bytes.push_back((val >> 32) & 255);
                data_bytes.push_back((val >> 40) & 255);
                data_bytes.push_back((val >> 48) & 255);
                data_bytes.push_back((val >> 56) & 255);
                break;
            }
            case nodes::DataType::_TYPE_FLOAT:
            {
                // 32-bit floats
                FLoat32 f32;
                data_addrs[data.first] = start;
                start += 4;
                f32.whole = std::stof(data.second.value);
                data_bytes.push_back(f32.b4);
                data_bytes.push_back(f32.b3);
                data_bytes.push_back(f32.b2);
                data_bytes.push_back(f32.b1);
                break;
            }
            case nodes::DataType::_TYPE_LFLOAT:
            {
                // 64-bit floats
                FLoat64 f64;
                data_addrs[data.first] = start;
                start += 8;
                f64.whole = std::stod(data.second.value);
                data_bytes.push_back(f64.b8);
                data_bytes.push_back(f64.b7);
                data_bytes.push_back(f64.b6);
                data_bytes.push_back(f64.b5);
                data_bytes.push_back(f64.b4);
                data_bytes.push_back(f64.b3);
                data_bytes.push_back(f64.b2);
                data_bytes.push_back(f64.b1);
                break;
            }
            }
            break;
        }
        }
    }
    // we need to do double loop since the strings go at last
    if ((data_bytes.size() % 8) != 0)
    {
        while ((data_bytes.size() % 8) != 0)
        {
            data_bytes.push_back(0);
            start++;
        }
    }
    for (auto data : table)
    {
        switch (data.second.type)
        {
        case symtable::SymEntryType::_VAR:
        {
            if (data.second.dtype == nodes::DataType::_TYPE_STRING)
            {
                data_addrs[data.first] = start;
                start += data.second.value.length();
                for (auto c : data.second.value)
                {
                    data_bytes.push_back(c);
                    str_start_len++;
                }
                data_bytes.push_back(0);
                str_start_len++;
            }
            break;
        }
        }
    }
    if ((data_bytes.size() % 8) != 0)
    {
        while ((data_bytes.size() % 8) != 0)
        {
            data_bytes.push_back(0);
            str_start_len++;
        }
    }
}

void zeta::codegen::Codegen::gen_inst_mov_reg_reg(std::unique_ptr<nodes::Node> &node, size_t bytes)
{
    auto n = (nodes::NodeInstMovRegReg *)node->ptr.get();
    Instruction inst;
    inst.bytes.b1 = bytes == 8 ? opcodes::OP_MOVE_REG : bytes == 4 ? opcodes::OP_MOVE_REG32
                                                    : bytes == 2   ? opcodes::OP_MOVE_REG16
                                                                   : opcodes::OP_MOVE_REG8;
    inst.bytes.b8 = (n->dest_regr << 4) | n->src_reg;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_move(std::unique_ptr<nodes::Node> &node, size_t bytes)
{
    auto n = (nodes::NodeInstMovRegReg *)node->ptr.get();
    Instruction inst;
    inst.bytes.b1 = bytes == 4 ? opcodes::OP_MOV32 : bytes == 2 ? opcodes::OP_MOV16
                                                                : opcodes::OP_MOV8;
    inst.bytes.b8 = (n->dest_regr << 4) | n->src_reg;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_mov_reg_immq(std::unique_ptr<nodes::Node> &node)
{
    nodes::NodeInstMovRegImm *n = (nodes::NodeInstMovRegImm *)node->ptr.get();
    Instruction final_inst;
    bool imm_needed = false;
    if (n->is_iden)
    {
        size_t addr_of_iden = data_addrs.find(n->value)->second;
        auto iden_details = table.find_entry(n->value)->second;
        // movq only accepts 8 bytes long numbers
        final_inst.bytes.b1 = opcodes::OP_LOAD;
        final_inst.bytes.b2 = (n->dest_regr);
        final_inst.whole |= addr_of_iden;
    }
    else
    {
        final_inst.bytes.b1 = opcodes::OP_MOVE_IMM_64;
        final_inst.bytes.b8 = n->dest_regr;
        imm_needed = true;
    }
    inst_bytes.push_back(final_inst);
    // now we need to push the immediate as well
    if (imm_needed)
    {
        Instruction imm;
        imm.whole = std::stoull(n->value);
        inst_bytes.push_back(imm);
    }
}

void zeta::codegen::Codegen::gen_inst_mov_reg_imm(std::unique_ptr<nodes::Node> &node)
{
    // based on the situation we need to
    nodes::NodeInstMovRegImm *n = (nodes::NodeInstMovRegImm *)node->ptr.get();
    Instruction final_inst;
    if (n->is_iden)
    {
        // it is an identifier
        // get info on the identifier
        size_t addr_of_iden = data_addrs.find(n->value)->second;
        // we can be sure that it is correctly defined
        // also need to check the size of the data
        auto iden_details = table.find_entry(n->value)->second;
        switch (iden_details.dtype)
        {
        case nodes::DataType::_TYPE_STRING:
        case nodes::DataType::_TYPE_BYTE:
        {
            // this is a byte type value so we need to use loadb instruction
            final_inst.bytes.b1 = opcodes::OP_LOADB;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            // we then need to push the 6 byte address of the
            break;
        }
        case nodes::DataType::_TYPE_WORD:
        {
            final_inst.bytes.b1 = opcodes::OP_LOADW;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            break;
        }
        case nodes::DataType::_TYPE_DWORD:
        {
            final_inst.bytes.b1 = opcodes::OP_LOADD;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            break;
        }
        case nodes::DataType::_TYPE_QWORD:
        {
            final_inst.bytes.b1 = opcodes::OP_LOAD;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            break;
        }
        }
    }
    else
    {
        // we just have a plain immediate
        // but we still need to know the size of the immediate
        // for moving 64-bit immdiate, we will have a dedicated instruction but the general move_imm should suffice
        final_inst.bytes.b1 = opcodes::OP_MOVE_IMM;
        final_inst.bytes.b2 = n->dest_regr;
        final_inst.whole |= std::stoull(n->value) & 0xFFFFFFFF;
    }
    inst_bytes.push_back(final_inst);
}

void zeta::codegen::Codegen::gen_inst_movsx_reg_reg(std::unique_ptr<nodes::Node> &node, size_t bytes)
{
    auto n = (nodes::NodeInstMovRegReg *)node->ptr.get();
    Instruction inst;
    inst.bytes.b1 = bytes == 4 ? opcodes::OP_MOVESX_REG32 : bytes == 2 ? opcodes::OP_MOVESX_REG16
                                                                       : opcodes::OP_MOVESX_REG8;
    inst.bytes.b8 = (n->dest_regr << 4) | n->src_reg;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_movsx_reg_imm(std::unique_ptr<nodes::Node> &node, size_t size)
{
    nodes::NodeInstMovRegImm *n = (nodes::NodeInstMovRegImm *)node->ptr.get();
    Instruction final_inst;
    if (n->is_iden)
    {
        size_t addr_of_iden = data_addrs.find(n->value)->second;
        auto iden_details = table.find_entry(n->value)->second;
        switch (iden_details.dtype)
        {
        case nodes::DataType::_TYPE_BYTE:
        {
            final_inst.bytes.b1 = opcodes::OP_LOADB;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            inst_bytes.push_back(final_inst);
            final_inst.bytes.b1 = opcodes::OP_MOVESX_REG8;
            final_inst.bytes.b8 = (n->dest_regr);
            final_inst.bytes.b8 <<= 4;
            final_inst.bytes.b8 |= n->dest_regr;
            // we then need to push the 6 byte address of the
            break;
        }
        case nodes::DataType::_TYPE_WORD:
        {
            final_inst.bytes.b1 = opcodes::OP_LOADW;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            inst_bytes.push_back(final_inst);
            final_inst.bytes.b1 = opcodes::OP_MOVESX_REG16;
            final_inst.bytes.b8 = (n->dest_regr);
            final_inst.bytes.b8 <<= 4;
            final_inst.bytes.b8 |= n->dest_regr;
            break;
        }
        case nodes::DataType::_TYPE_DWORD:
        {
            final_inst.bytes.b1 = opcodes::OP_LOADD;
            final_inst.bytes.b2 = (n->dest_regr);
            final_inst.whole |= addr_of_iden;
            inst_bytes.push_back(final_inst);
            final_inst.bytes.b1 = opcodes::OP_MOVESX_REG32;
            final_inst.bytes.b8 = (n->dest_regr);
            final_inst.bytes.b8 <<= 4;
            final_inst.bytes.b8 |= n->dest_regr;
            break;
        }
        }
    }
    else
    {
        // we just have a plain immediate
        // but we still need to know the size of the immediate
        // for moving 64-bit immdiate, we will have a dedicated instruction but the general move_imm should suffice
        final_inst.bytes.b1 = size == 4 ? opcodes::OP_MOVESX_REG32 : size == 2 ? opcodes::OP_MOVESX_IMM16
                                                                               : opcodes::OP_MOVESX_IMM8; // this is 32-bit by default
        final_inst.bytes.b2 = n->dest_regr;
        final_inst.whole |= (std::stoull(n->value) & 0xFFFFFFFF);
    }
    inst_bytes.push_back(final_inst);
}

void zeta::codegen::Codegen::label_labels()
{
    // just go through each instruction
    // see if any label is there
    // then add it to the label_addrs
    size_t i = 0;
    for (auto &x : inst_nodes)
    {
        if (x->kind == nodes::NodeKind::_LABEL)
            label_addrs[((nodes::NodeLabel *)x->ptr.get())->label_name] = i;
        else if (x->kind == nodes::NodeKind::_INST_MOV_REG_IMMQ)
        {
            if (((nodes::NodeInstMovRegImm *)x->ptr.get())->is_iden)
                i += 2;
        }
        else if (x->kind == nodes::NodeKind::_INST_MOVSX_REG_IMM32 || x->kind == nodes::NodeKind::_INST_MOVSX_REG_IMM16 || x->kind == nodes::NodeKind::_INST_MOVSX_REG_IMM8)
        {
            if (((nodes::NodeInstMovRegImm *)x->ptr.get())->is_iden)
                i += 2;
        }
        else if (x->kind == nodes::NodeKind::_INST_MOVF || x->kind == nodes::NodeKind::_INST_MOVLF)
        {
            if (((nodes::NodeInstMovRegImm *)x->ptr.get())->is_iden && x->kind == nodes::NodeKind::_INST_MOVLF)
                i += 2;
        }
        else if (x->kind == nodes::NodeKind::_INST_CMP_IMM)
        {
            if (!((nodes::NodeCmpImm *)x->ptr.get())->is_iden)
                i += 2;
        }
        else if (x->kind > nodes::NodeKind::_LABEL)
            i++;
    }
}

void zeta::codegen::Codegen::gen_inst_cin(std::unique_ptr<nodes::Node> &node)
{
    nodes::NodeOneRegrOperands *inst = (nodes::NodeOneRegrOperands *)node->ptr.get();
    Instruction i;
    i.bytes.b1 = node->kind == nodes::NodeKind::_INST_CIN ? opcodes::OP_CIN : opcodes::OP_CIN;
    i.bytes.b8 = inst->oper_rger;
    inst_bytes.push_back(i);
}

void zeta::codegen::Codegen::gen_inst_cout(std::unique_ptr<nodes::Node> &node)
{
    nodes::NodeOneRegrOperands *inst = (nodes::NodeOneRegrOperands *)node->ptr.get();
    Instruction i;
    i.bytes.b1 = node->kind == nodes::NodeKind::_INST_COUT ? opcodes::OP_COUT : opcodes::OP_COUT;
    i.bytes.b8 = inst->oper_rger;
    inst_bytes.push_back(i);
}

void zeta::codegen::Codegen::gen_inst_sin(size_t address)
{
    Instruction inst;
    inst.bytes.b1 = opcodes::OP_SIN;
    inst.bytes.b3 = (address >> 40) & 255;
    inst.bytes.b4 = (address >> 32) & 255;
    inst.bytes.b5 = (address >> 24) & 255;
    inst.bytes.b6 = (address >> 16) & 255;
    inst.bytes.b7 = (address >> 8) & 255;
    inst.bytes.b8 = (address) & 255;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_sout(size_t address)
{
    Instruction inst;
    inst.bytes.b1 = opcodes::OP_SOUT;
    inst.bytes.b3 = (address >> 40) & 255;
    inst.bytes.b4 = (address >> 32) & 255;
    inst.bytes.b5 = (address >> 24) & 255;
    inst.bytes.b6 = (address >> 16) & 255;
    inst.bytes.b7 = (address >> 8) & 255;
    inst.bytes.b8 = (address) & 255;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_movf(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    auto i = (nodes::NodeInstMovRegImm *)node->ptr.get();
    bool imm_needed = false;
    if (i->is_iden)
    {
        size_t addr_of_iden = data_addrs.find(i->value)->second;
        auto iden_details = table.find_entry(i->value)->second;
        switch (iden_details.dtype)
        {
        case nodes::DataType::_TYPE_FLOAT:
        {
            inst.bytes.b1 = opcodes::OP_LOADD;
            inst.bytes.b2 = (i->dest_regr);
            inst.whole |= addr_of_iden;
            break;
        }
        case nodes::DataType::_TYPE_LFLOAT:
        {
            inst.bytes.b1 = opcodes::OP_LOAD;
            inst.bytes.b2 = (i->dest_regr);
            inst.whole |= addr_of_iden;
            break;
        }
        }
    }
    else
    {
        if (node->kind == nodes::NodeKind::_INST_MOVF)
            inst.bytes.b1 = opcodes::OP_MOVE_IMM;
        else
        {
            inst.bytes.b1 = opcodes::OP_MOVE_IMM_64;
            imm_needed = true;
        }
        inst.bytes.b2 = i->dest_regr;
        if (!imm_needed)
        {
            FLoat32 f32;
            f32.whole = std::stof(i->value);
            inst.whole |= f32.in_int;
        }
    }
    inst_bytes.push_back(inst);
    if (imm_needed)
    {
        FLoat64 f64;
        f64.whole = std::stod(i->value);
        inst.whole = f64.in_int;
        inst_bytes.push_back(inst);
    }
}

void zeta::codegen::Codegen::gen_inst_add(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_ADD_IMM || node->kind == nodes::_INST_IADD_IMM)
    {
        auto i = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        if (i->is_iden)
        {
            size_t addr_of_iden = data_addrs.find(i->value)->second;
            auto iden_details = table.find_entry(i->value)->second;
            switch (iden_details.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_ADD_MEMB;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_ADD_MEMW;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_ADD_MEMD;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_ADD_MEMQ;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            }
        }
        else
        {
            // it is an immediate number
            // it must be an integer which is confirmed by sema
            inst.bytes.b1 = node->kind == nodes::_INST_ADD_IMM ? opcodes::OP_ADD_IMM : opcodes::OP_IADD_IMM;
            inst.bytes.b2 = i->dest_regr;
            inst.whole |= std::stoi(i->value);
        }
    }
    else
    {
        // must be a add_regr
        auto i = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = (node->kind == nodes::_INST_ADD_REG) ? opcodes::OP_ADD_REG : opcodes::OP_IADD_REG;
        inst.bytes.b8 = i->dest_regr;
        (inst.bytes.b8 <<= 4) |= i->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_sub(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_SUB_IMM || node->kind == nodes::_INST_ISUB_IMM)
    {
        auto i = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        if (i->is_iden)
        {
            size_t addr_of_iden = data_addrs.find(i->value)->second;
            auto iden_details = table.find_entry(i->value)->second;
            switch (iden_details.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_SUB_MEMB;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_SUB_MEMW;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_SUB_MEMD;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_SUB_MEMQ;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            }
        }
        else
        {
            inst.bytes.b1 = node->kind == nodes::_INST_SUB_IMM ? opcodes::OP_SUB_IMM : opcodes::OP_ISUB_IMM;
            inst.bytes.b2 = i->dest_regr;
            inst.whole |= std::stoi(i->value);
        }
    }
    else
    {
        auto i = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::_INST_SUB_REG ? opcodes::OP_SUB_REG : opcodes::OP_ISUB_REG;
        inst.bytes.b8 = i->dest_regr;
        (inst.bytes.b8 <<= 4) |= i->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_mul(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_MUL_IMM || node->kind == nodes::_INST_IMUL_IMM)
    {
        auto i = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        if (i->is_iden)
        {
            size_t addr_of_iden = data_addrs.find(i->value)->second;
            auto iden_details = table.find_entry(i->value)->second;
            switch (iden_details.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_MUL_MEMB;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_MUL_MEMW;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_MUL_MEMD;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_MUL_MEMQ;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            }
        }
        else
        {
            inst.bytes.b1 = node->kind == nodes::_INST_MUL_IMM ? opcodes::OP_MUL_IMM : opcodes::OP_IMUL_IMM;
            inst.bytes.b2 = i->dest_regr;
            inst.whole |= std::stoi(i->value);
        }
    }
    else
    {
        auto i = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::_INST_MUL_REG ? opcodes::OP_MUL_REG : opcodes::OP_IMUL_REG;
        inst.bytes.b8 = i->dest_regr;
        (inst.bytes.b8 <<= 4) |= i->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_div(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_DIV_IMM || node->kind == nodes::_INST_IDIV_IMM)
    {
        auto i = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        if (i->is_iden)
        {
            size_t addr_of_iden = data_addrs.find(i->value)->second;
            auto iden_details = table.find_entry(i->value)->second;
            switch (iden_details.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_DIV_MEMB;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_DIV_MEMW;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_DIV_MEMD;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_DIV_MEMQ;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            }
        }
        else
        {
            inst.bytes.b1 = node->kind == nodes::_INST_DIV_IMM ? opcodes::OP_DIV_IMM : opcodes::OP_IDIV_IMM;
            inst.bytes.b2 = i->dest_regr;
            inst.whole |= std::stoi(i->value);
        }
    }
    else
    {
        auto i = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::_INST_DIV_REG ? opcodes::OP_DIV_REG : opcodes::OP_IDIV_REG;
        inst.bytes.b8 = i->dest_regr;
        (inst.bytes.b8 <<= 4) |= i->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_mod(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_MOD_IMM || node->kind == nodes::_INST_IMOD_IMM)
    {
        auto i = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        if (i->is_iden)
        {
            size_t addr_of_iden = data_addrs.find(i->value)->second;
            auto iden_details = table.find_entry(i->value)->second;
            switch (iden_details.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_MOD_MEMB;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_MOD_MEMW;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_MOD_MEMD;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_MOD_MEMQ;
                inst.bytes.b2 = i->dest_regr;
                inst.whole |= addr_of_iden;
                break;
            }
            }
        }
        else
        {
            inst.bytes.b1 = node->kind == nodes::_INST_MOD_IMM ? opcodes::OP_MOD_IMM : opcodes::OP_IMOD_IMM;
            inst.bytes.b2 = i->dest_regr;
            inst.whole |= std::stoi(i->value);
        }
    }
    else
    {
        auto i = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::_INST_MOD_REG ? opcodes::OP_MOD_REG : opcodes::OP_IMOD_REG;
        inst.bytes.b8 = i->dest_regr;
        (inst.bytes.b8 <<= 4) |= i->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_fadd(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::NodeKind::_INST_FADD_IMM || node->kind == nodes::NodeKind::_INST_LFADD_IMM)
    {
        auto temp = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        // we need to get the variable from the memory first
        size_t addr_of_iden = data_addrs.find(temp->value)->second;
        auto iden_details = table.find_entry(temp->value)->second;
        // added new instructions for this purpose
        if (iden_details.dtype == nodes::DataType::_TYPE_FLOAT)
            inst.bytes.b1 = opcodes::OP_FADD32_MEM;
        else
            inst.bytes.b1 = opcodes::OP_FADD_MEM;
        inst.whole |= addr_of_iden;
    }
    else
    {
        // we have two registers here
        auto temp = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::NodeKind::_INST_FADD_REG ? opcodes::OP_FADD32 : opcodes::OP_FADD;
        inst.bytes.b8 = temp->dest_regr;
        (inst.bytes.b8 <<= 4) |= temp->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_fsub(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::NodeKind::_INST_FSUB_IMM || node->kind == nodes::NodeKind::_INST_LFSUB_IMM)
    {
        auto temp = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        size_t addr_of_iden = data_addrs.find(temp->value)->second;
        auto iden_details = table.find_entry(temp->value)->second;
        if (iden_details.dtype == nodes::DataType::_TYPE_FLOAT)
            inst.bytes.b1 = opcodes::OP_FSUB32_MEM;
        else
            inst.bytes.b1 = opcodes::OP_FSUB_MEM;
        inst.whole |= addr_of_iden;
    }
    else
    {
        auto temp = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::NodeKind::_INST_FSUB_REG ? opcodes::OP_FSUB32 : opcodes::OP_FSUB;
        inst.bytes.b8 = temp->dest_regr;
        (inst.bytes.b8 <<= 4) |= temp->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_fmul(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::NodeKind::_INST_FMUL_IMM || node->kind == nodes::NodeKind::_INST_LFMUL_IMM)
    {
        auto temp = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        size_t addr_of_iden = data_addrs.find(temp->value)->second;
        auto iden_details = table.find_entry(temp->value)->second;
        if (iden_details.dtype == nodes::DataType::_TYPE_FLOAT)
            inst.bytes.b1 = opcodes::OP_FMUL32_MEM;
        else
            inst.bytes.b1 = opcodes::OP_FMUL_MEM;
        inst.whole |= addr_of_iden;
    }
    else
    {
        auto temp = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::NodeKind::_INST_FMUL_REG ? opcodes::OP_FMUL32 : opcodes::OP_FMUL;
        inst.bytes.b8 = temp->dest_regr;
        (inst.bytes.b8 <<= 4) |= temp->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_fdiv(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::NodeKind::_INST_FDIV_IMM || node->kind == nodes::NodeKind::_INST_LFDIV_IMM)
    {
        auto temp = (nodes::NodeArithmeticRegImm *)node->ptr.get();
        size_t addr_of_iden = data_addrs.find(temp->value)->second;
        auto iden_details = table.find_entry(temp->value)->second;
        if (iden_details.dtype == nodes::DataType::_TYPE_FLOAT)
            inst.bytes.b1 = opcodes::OP_FDIV32_MEM;
        else
            inst.bytes.b1 = opcodes::OP_FDIV_MEM;
        inst.whole |= addr_of_iden;
    }
    else
    {
        auto temp = (nodes::NodeArithmeticRegReg *)node->ptr.get();
        inst.bytes.b1 = node->kind == nodes::NodeKind::_INST_FDIV_REG ? opcodes::OP_FDIV32 : opcodes::OP_FDIV;
        inst.bytes.b8 = temp->dest_regr;
        (inst.bytes.b8 <<= 4) |= temp->src_reg;
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_jmp(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    auto temp = (nodes::NodeControlFlow *)node->ptr.get();
    size_t address = label_addrs.find(temp->_jmp_label_)->second;
    // since the label exists and we have made sure that it does
    // we can continue without problems
    inst.bytes.b1 = opcodes::OP_JMP_ADDR;
    inst.whole |= address;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_call(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    auto temp = (nodes::NodeControlFlow *)node->ptr.get();
    size_t address = label_addrs.find(temp->_jmp_label_)->second;
    inst.bytes.b1 = opcodes::OP_CALL;
    inst.whole |= address;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_jX(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    auto temp = (nodes::NodeControlFlow *)node->ptr.get();
    size_t address = label_addrs.find(temp->_jmp_label_)->second;
    inst.bytes.b1 = (opcodes::opcodes)(node->kind - 21);
    inst.whole |= address;
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_cmp(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    if (node->kind == nodes::_INST_CMP_REG)
    {
        auto x = (nodes::NodeCmpRegr *)node->ptr.get();
        inst.bytes.b1 = opcodes::OP_CMP_REG;
        inst.bytes.b8 = x->regr1;
        (inst.bytes.b8 <<= 4) |= x->regr2;
    }
    else
    {
        auto x = (nodes::NodeCmpImm *)node->ptr.get();
        if (!x->is_iden)
        {
            inst.bytes.b1 = opcodes::OP_CMP_IMM;
            inst_bytes.push_back(inst);
            inst.whole = 0;
            inst.whole = std::stoll(x->val);
        }
        else
        {
            auto var_details = table.find_entry(x->val);
            switch (var_details->second.dtype)
            {
            case nodes::DataType::_TYPE_BYTE:
            {
                inst.bytes.b1 = opcodes::OP_CMP_IMM_MEMB;
                inst.bytes.b2 = x->regr;
                inst.whole |= data_addrs.find(x->val)->second;
                break;
            }
            case nodes::DataType::_TYPE_WORD:
            {
                inst.bytes.b1 = opcodes::OP_CMP_IMM_MEMW;
                inst.bytes.b2 = x->regr;
                inst.whole |= data_addrs.find(x->val)->second;
                break;
            }
            case nodes::DataType::_TYPE_DWORD:
            {
                inst.bytes.b1 = opcodes::OP_CMP_IMM_MEMD;
                inst.bytes.b2 = x->regr;
                inst.whole |= data_addrs.find(x->val)->second;
                break;
            }
            case nodes::DataType::_TYPE_QWORD:
            {
                inst.bytes.b1 = opcodes::OP_CMP_IMM_MEMQ;
                inst.bytes.b2 = x->regr;
                inst.whole |= data_addrs.find(x->val)->second;
                break;
            }
            }
        }
    }
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen_inst_sva_svc(std::unique_ptr<nodes::Node> &node)
{
    Instruction inst;
    auto x = (nodes::NodeOneRegrOneImm *)node->ptr.get();
    inst.bytes.b1 = node->kind == nodes::_INST_SVA ? opcodes::OP_SVA : opcodes::OP_SVC;
    inst.bytes.b2 = x->regr;
    inst.whole |= std::stoi(x->imm);
    inst_bytes.push_back(inst);
}

void zeta::codegen::Codegen::gen()
{
    gen_data(); // generate data bytes
    label_labels();
    // size_t count = 0; // the number of instructions; also works as an address
    std::vector<std::unique_ptr<nodes::Node>>::iterator iter = inst_nodes.begin();
    auto temp = iter;
    while (iter != inst_nodes.end())
    {
        auto i = iter->get();
        switch (i->kind)
        {
        case nodes::NodeKind::_INST_SET_EXCP:
        {
            Instruction inst;
            auto temp = (nodes::NodeControlFlow *)i->ptr.get();
            size_t address = label_addrs.find(temp->_jmp_label_)->second;
            inst.bytes.b1 = opcodes::OP_SET_EXCP;
            inst.whole |= address;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CALL_EXCP:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CALL_EXCP;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CMPXCHG:
        {
            Instruction inst;
            auto temp = (nodes::NodeCmpXchg *)(*iter)->ptr.get();
            inst.bytes.b1 = opcodes::OP_CMPXCHG;
            inst.bytes.b2 = temp->r1;
            (inst.bytes.b2 <<= 4) |= temp->r2;
            inst.whole |= data_addrs.find(temp->var_name)->second;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_LOOP:
        {
            Instruction inst;
            auto temp = (nodes::NodeControlFlow *)(*iter)->ptr.get();
            size_t address = label_addrs.find(temp->_jmp_label_)->second;
            inst.bytes.b1 = opcodes::OP_LOOP;
            inst.whole |= address;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_LEA:
        {
            Instruction inst;
            auto temp = (nodes::NodeLea *)(*iter).get()->ptr.get();
            inst.bytes.b1 = opcodes::OP_LEA;
            inst.bytes.b8 = temp->scale;
            inst.bytes.b7 = temp->index;
            inst.bytes.b6 = temp->base;
            inst.bytes.b5 = temp->dest;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_STORE:
        case nodes::NodeKind::_INST_STOREB:
        case nodes::NodeKind::_INST_STOREW:
        case nodes::NodeKind::_INST_STORED:
        {
            Instruction inst;
            auto temp = (nodes::NodeStore *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_STORE ? opcodes::OP_STORE : i->kind == nodes::_INST_STOREB ? opcodes::OP_STOREB
                                                                            : i->kind == nodes::_INST_STOREW   ? opcodes::OP_STOREW
                                                                                                               : opcodes::OP_STORED;
            inst.bytes.b2 = temp->dest;
            inst.whole |= data_addrs[temp->var_name];
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_ATM_STORE:
        case nodes::NodeKind::_INST_ATM_STOREB:
        case nodes::NodeKind::_INST_ATM_STOREW:
        case nodes::NodeKind::_INST_ATM_STORED:
        {
            Instruction inst;
            auto temp = (nodes::NodeStore *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_ATM_STORE ? opcodes::OP_ATOMIC_STORE : i->kind == nodes::_INST_ATM_STOREB ? opcodes::OP_ATOMIC_STOREB
                                                                                       : i->kind == nodes::_INST_ATM_STOREW   ? opcodes::OP_ATOMIC_STOREW
                                                                                                                              : opcodes::OP_ATOMIC_STORED;
            inst.bytes.b2 = temp->dest;
            inst.whole |= data_addrs.find(temp->var_name)->second;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_LOAD:
        case nodes::NodeKind::_INST_LOADB:
        case nodes::NodeKind::_INST_LOADW:
        case nodes::NodeKind::_INST_LOADD:
        {
            Instruction inst;
            auto temp = (nodes::NodeLoad *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_LOAD ? opcodes::OP_LOAD : i->kind == nodes::_INST_LOADB ? opcodes::OP_LOADB
                                                                          : i->kind == nodes::_INST_LOADW   ? opcodes::OP_LOADW
                                                                                                            : opcodes::OP_LOADD;
            inst.bytes.b2 = temp->dest;
            inst.whole |= data_addrs.find(temp->var_name)->second;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_ATM_LOAD:
        case nodes::NodeKind::_INST_ATM_LOADB:
        case nodes::NodeKind::_INST_ATM_LOADW:
        case nodes::NodeKind::_INST_ATM_LOADD:
        {
            Instruction inst;
            auto temp = (nodes::NodeLoad *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_ATM_LOAD ? opcodes::OP_ATOMIC_LOAD : i->kind == nodes::_INST_ATM_LOADB ? opcodes::OP_ATOMIC_LOADB
                                                                                     : i->kind == nodes::_INST_ATM_LOADW   ? opcodes::OP_ATOMIC_LOADW
                                                                                                                           : opcodes::OP_ATOMIC_LOADD;
            inst.bytes.b2 = temp->dest;
            inst.whole |= data_addrs.find(temp->var_name)->second;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_LOAD_REG:
        case nodes::NodeKind::_INST_LOADB_REG:
        case nodes::NodeKind::_INST_LOADW_REG:
        case nodes::NodeKind::_INST_LOADD_REG:
        {
            Instruction inst;
            auto temp = (nodes::NodeLoadReg *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_LOAD_REG ? opcodes::OP_LOAD_REG : i->kind == nodes::_INST_LOADB_REG ? opcodes::OP_LOADB_REG
                                                                                  : i->kind == nodes::_INST_LOADW_REG   ? opcodes::OP_LOADW_REG
                                                                                                                        : opcodes::OP_LOADD_REG;
            inst.bytes.b8 = temp->dest;
            inst.bytes.b8 <<= 4;
            inst.bytes.b8 |= temp->addr_regr;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_STORE_REG:
        case nodes::NodeKind::_INST_STOREB_REG:
        case nodes::NodeKind::_INST_STOREW_REG:
        case nodes::NodeKind::_INST_STORED_REG:
        {
            Instruction inst;
            auto temp = (nodes::NodeStoreReg *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_STORE_REG ? opcodes::OP_STORE_REG : i->kind == nodes::_INST_STOREB_REG ? opcodes::OP_STOREB_REG
                                                                                    : i->kind == nodes::_INST_STOREW_REG   ? opcodes::OP_STOREW_REG
                                                                                                                           : opcodes::OP_STORED_REG;
            inst.bytes.b8 = temp->dest;
            inst.bytes.b8 <<= 4;
            inst.bytes.b8 |= temp->addr_regr;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_EXCG:
        case nodes::NodeKind::_INST_EXCG16:
        case nodes::NodeKind::_INST_EXCG32:
        case nodes::NodeKind::_INST_EXCG8:
        {
            Instruction inst;
            auto temp = (nodes::NodeExcg *)(*iter).get()->ptr.get();
            inst.bytes.b1 = i->kind == nodes::_INST_EXCG16 ? opcodes::OP_EXCG16 : i->kind == nodes::_INST_EXCG32 ? opcodes::OP_EXCG32
                                                                              : i->kind == nodes::_INST_EXCG8    ? opcodes::OP_EXCG8
                                                                                                                 : opcodes::OP_EXCG;
            inst.bytes.b3 = temp->regr1;
            inst.bytes.b3 <<= 4;
            inst.bytes.b3 |= temp->regr2;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_PUSH_IMM:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_PUSH_IMM;
            inst.whole |= std::stoi(((nodes::NodeOneImmOperand *)(*iter).get())->imm);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_INTR:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INTR;
            inst.whole |= std::stoi(((nodes::NodeOneImmOperand *)(*iter).get())->imm);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_POP:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_POP;
            inst.whole |= (((nodes::NodeOneRegrOperands *)(*iter).get())->oper_rger);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_PUSH_REG:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_PUSH_REG;
            inst.whole |= (((nodes::NodeOneRegrOperands *)(*iter).get())->oper_rger);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_POPA:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_POPA;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_PUSHA:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_PUSHA;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_NOP:
        {
            inst_bytes.push_back(Instruction());
            break;
        }
        case nodes::NodeKind::_INST_HLT:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_HALT;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_SVA:
        case nodes::NodeKind::_INST_SVC:
            gen_inst_sva_svc(*iter);
            break;
        case nodes::NodeKind::_INST_MOV_REG_IMMQ:
        {
            gen_inst_mov_reg_immq(*iter);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMM:
        case nodes::NodeKind::_INST_MOV_REG_IMM8:
        case nodes::NodeKind::_INST_MOV_REG_IMM16:
        case nodes::NodeKind::_INST_MOV_REG_IMM32:
        {
            // we need to change this to load instruction
            // based on the operands
            gen_inst_mov_reg_imm(*iter);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_REG:
        {
            gen_inst_mov_reg_reg(*iter, 8);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_REG8:
        {
            gen_inst_mov_reg_reg(*iter, 1);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_REG16:
        {
            gen_inst_mov_reg_reg(*iter, 2);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_REG32:
        {
            gen_inst_mov_reg_reg(*iter, 4);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM8:
        {
            gen_inst_movsx_reg_imm(*iter, 1);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM16:
        {
            gen_inst_movsx_reg_imm(*iter, 2);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM32:
        {
            gen_inst_movsx_reg_imm(*iter, 4);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_REG8:
        {
            gen_inst_movsx_reg_reg(*iter, 1);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_REG16:
        {
            gen_inst_movsx_reg_reg(*iter, 2);
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_REG32:
        {
            gen_inst_movsx_reg_reg(*iter, 4);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_MOVEB:
        {
            gen_inst_move(*iter, 1);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_MOVEW:
        {
            gen_inst_move(*iter, 2);
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_MOVED:
        {
            gen_inst_move(*iter, 4);
            break;
        }
        case nodes::NodeKind::_INST_OUTR:
        {
            Instruction i;
            i.bytes.b1 = opcodes::OP_OUTR;
            inst_bytes.push_back(i);
            break;
        }
        case nodes::NodeKind::_INST_UOUTR:
        {
            Instruction i;
            i.bytes.b1 = opcodes::OP_UOUTR;
            inst_bytes.push_back(i);
            break;
        }
        case nodes::NodeKind::_INST_CIN:
        {
            gen_inst_cin(*iter);
            break;
        }
        case nodes::NodeKind::_INST_COUT:
        {
            gen_inst_cout(*iter);
            break;
        }
        case nodes::NodeKind::_INST_SIN:
        {
            gen_inst_sin(data_addrs[((nodes::NodeOneImmOperand *)(i->ptr.get()))->imm]);
            break;
        }
        case nodes::NodeKind::_INST_SOUT:
        {
            gen_inst_sout(data_addrs[((nodes::NodeOneImmOperand *)(i->ptr.get()))->imm]);
            break;
        }
        case nodes::NodeKind::_INST_IN:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_IN;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUT:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUT;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_INF:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INF32;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUTF:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUTF32;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_INLF:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INF;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUTLF:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUTF;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_INW:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INW;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUTW:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUTW;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_IND:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_IND;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUTD:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUTD;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_INQ:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INQ;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OUTQ:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OUTQ;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UIN:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UIN;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UOUT:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UOUT;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UINW:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UINW;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UOUTW:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UOUTW;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UIND:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UIND;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UOUTD:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UOUTD;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UINQ:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UINQ;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_UOUTQ:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_UOUTQ;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)iter->get()->ptr.get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_MOVF:
        case nodes::NodeKind::_INST_MOVLF:
        {
            gen_inst_movf(*iter);
            break;
        }
        case nodes::NodeKind::_INST_ADD_IMM:
        case nodes::NodeKind::_INST_ADD_REG:
        case nodes::NodeKind::_INST_IADD_IMM:
        case nodes::NodeKind::_INST_IADD_REG:
        {
            gen_inst_add(*iter);
            break;
        }
        case nodes::NodeKind::_INST_SUB_IMM:
        case nodes::NodeKind::_INST_SUB_REG:
        case nodes::NodeKind::_INST_ISUB_IMM:
        case nodes::NodeKind::_INST_ISUB_REG:
        {
            gen_inst_sub(*iter);
            break;
        }
        case nodes::NodeKind::_INST_MUL_IMM:
        case nodes::NodeKind::_INST_MUL_REG:
        case nodes::NodeKind::_INST_IMUL_IMM:
        case nodes::NodeKind::_INST_IMUL_REG:
        {
            gen_inst_mul(*iter);
            break;
        }
        case nodes::NodeKind::_INST_DIV_IMM:
        case nodes::NodeKind::_INST_DIV_REG:
        case nodes::NodeKind::_INST_IDIV_IMM:
        case nodes::NodeKind::_INST_IDIV_REG:
        {
            gen_inst_div(*iter);
            break;
        }
        case nodes::NodeKind::_INST_MOD_IMM:
        case nodes::NodeKind::_INST_MOD_REG:
        case nodes::NodeKind::_INST_IMOD_REG:
        case nodes::NodeKind::_INST_IMOD_IMM:
        {
            gen_inst_mod(*iter);
            break;
        }
        case nodes::NodeKind::_INST_FADD_IMM:
        case nodes::NodeKind::_INST_FADD_REG:
            gen_inst_fadd(*iter);
            break;
        case nodes::NodeKind::_INST_FSUB_IMM:
        case nodes::NodeKind::_INST_FSUB_REG:
            gen_inst_fsub(*iter);
            break;
        case nodes::NodeKind::_INST_FMUL_IMM:
        case nodes::NodeKind::_INST_FMUL_REG:
            gen_inst_fmul(*iter);
            break;
        case nodes::NodeKind::_INST_FDIV_IMM:
        case nodes::NodeKind::_INST_FDIV_REG:
            gen_inst_fdiv(*iter);
            break;
        case nodes::NodeKind::_INST_JMP:
            gen_inst_jmp(*iter);
            break;
        case nodes::NodeKind::_INST_CALL:
            gen_inst_call(*iter);
            break;
        case nodes::NodeKind::_INST_JNZ:
        case nodes::NodeKind::_INST_JZ:
        case nodes::NodeKind::_INST_JNE:
        case nodes::NodeKind::_INST_JE:
        case nodes::NodeKind::_INST_JNC:
        case nodes::NodeKind::_INST_JC:
        case nodes::NodeKind::_INST_JNO:
        case nodes::NodeKind::_INST_JO:
        case nodes::NodeKind::_INST_JNN:
        case nodes::NodeKind::_INST_JN:
        case nodes::NodeKind::_INST_JNG:
        case nodes::NodeKind::_INST_JG:
        case nodes::NodeKind::_INST_JNS:
        case nodes::NodeKind::_INST_JS:
        case nodes::NodeKind::_INST_JGE:
        case nodes::NodeKind::_INST_JSE:
            gen_inst_jX(*iter);
            break;
        case nodes::NodeKind::_INST_CMP_IMM:
        case nodes::NodeKind::_INST_CMP_REG:
            gen_inst_cmp(*iter);
            break;
        case nodes::NodeKind::_INST_INC:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_INC;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)(*iter).get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_DEC:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_DEC;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)(*iter).get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_NOT:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_NOT;
            inst.bytes.b8 = ((nodes::NodeOneRegrOperands *)(*iter).get())->oper_rger;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_LSHIFT:
        {
            Instruction inst;
            auto x = ((nodes::NodeShifts *)(*iter).get());
            inst.bytes.b1 = opcodes::OP_LSHIFT;
            inst.bytes.b8 = std::stoi(x->value);
            inst.bytes.b7 = x->dest_regr;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_RSHIFT:
        {
            Instruction inst;
            auto x = ((nodes::NodeShifts *)(*iter).get());
            inst.bytes.b1 = opcodes::OP_RSHIFT;
            inst.bytes.b8 = std::stoi(x->value);
            inst.bytes.b7 = x->dest_regr;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_AND_IMM:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_AND_IMM;
            auto x = ((nodes::NodeLogicalRegImm *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            inst_bytes.push_back(inst);
            inst.whole = std::stoull(x->value);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_AND_REG:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_AND_REG;
            auto x = ((nodes::NodeLogicalRegReg *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            (inst.bytes.b8 <<= 4) | x->src_reg;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OR_IMM:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OR_IMM;
            auto x = ((nodes::NodeLogicalRegImm *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            inst_bytes.push_back(inst);
            inst.whole = std::stoull(x->value);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_OR_REG:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_OR_REG;
            auto x = ((nodes::NodeLogicalRegReg *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            (inst.bytes.b8 <<= 4) | x->src_reg;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_XOR_IMM:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_XOR_IMM;
            auto x = ((nodes::NodeLogicalRegImm *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            inst_bytes.push_back(inst);
            inst.whole = std::stoull(x->value);
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_XOR_REG:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_XOR_REG;
            auto x = ((nodes::NodeLogicalRegReg *)(*iter).get());
            inst.bytes.b8 = x->dest_regr;
            (inst.bytes.b8 <<= 4) | x->src_reg;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CFLAGS:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CFLAGS;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_RESET:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_RESET;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CLC:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CLC;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CLN:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CLN;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CLO:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CLN;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_CLZ:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_CLZ;
            inst_bytes.push_back(inst);
            break;
        }
        case nodes::NodeKind::_INST_RET:
        {
            Instruction inst;
            inst.bytes.b1 = opcodes::OP_RET;
            inst_bytes.push_back(inst);
            break;
        }
            // default:
            //     count--;
        }
        // we don't care about procedure declaration right now
        iter++;
        // count++;
    }
}