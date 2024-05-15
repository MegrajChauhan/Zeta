#include "../includes/sema.hpp"

/*
 Why redundancy?
 All the repeating statements in the case statements can be easily moved to a function or made such that it is written only once.
 However, every instruction has it's own demand and so devising a way to handle each would make the code less readable, harder to understand and maintain.
 So we will stick of redundant code.
*/

// Given this is the final stage for errors to be found, Codegen won't need any error handling mechanisms

void zeta::sema::Sema::check_proc_declr()
{
    bool found_main = false;
    for (auto x : symtable)
    {
        if (x.second.type == symtable::SymEntryType::_PROC)
        {
            if (x.second.defined != true)
            {
                _l._register_sema_error_(x.second.line, "No procedure definition found for the procedure " + x.first, error::_NO_PROC_DEFN_);
                _set_();
            }
            if (x.first == "main")
                found_main = true;
        }
    }
    if (!found_main)
    {
        _l._register_sema_error_(0, "The definition for main procedure was not found", error::_NO_MAIN_DEFN_);
        _set_();
    }
}

void zeta::sema::Sema::gen_symtable()
{
    for (auto &node : nodes)
    {
        switch (node->type)
        {
        // as there can only be two types, the other is default
        // though we also need to take care of the procedure declaration
        case nodes::NodeType::_TYPE_DATA:
        {
            // if it is a data
            // just check and push it
            switch (node->kind)
            {
            case nodes::NodeKind::_DEF_BYTE:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                // check if this label already exists
                // if so then it is not allowed to redefine the same label twice
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_BYTE, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_WORD:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_WORD, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_DWORD:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_DWORD, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_FLOAT:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_FLOAT, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_LFLOAT:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_LFLOAT, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_QWORD:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_QWORD, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_STRING:
            {
                auto var = (nodes::NodeDefs *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->byte_name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->byte_name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->byte_name, symtable::SymTableEntry(symtable::_VAR, var->byte_val, nodes::_TYPE_STRING, 0, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_RESB:
            {
                auto var = (nodes::NodeRes *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->name, symtable::SymTableEntry(symtable::_VAR, "", nodes::_TYPE_RESB, var->number, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_RESW:
            {
                auto var = (nodes::NodeRes *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->name, symtable::SymTableEntry(symtable::_VAR, "", nodes::_TYPE_RESW, var->number, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_RESD:
            {
                auto var = (nodes::NodeRes *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->name, symtable::SymTableEntry(symtable::_VAR, "", nodes::_TYPE_RESD, var->number, node->line));
                break;
            }
            case nodes::NodeKind::_DEF_RESQ:
            {
                auto var = (nodes::NodeRes *)node->ptr.get();
                if (symtable.is_valid(symtable.find_entry(var->name)))
                {
                    _l._register_sema_error_(node->line, "Redefinition of variable " + var->name + " when it already exists.", error::_VAR_REDFIN_);
                    _set_();
                    break;
                }
                symtable.add_entry(var->name, symtable::SymTableEntry(symtable::_VAR, "", nodes::_TYPE_RESQ, var->number, node->line));
                break;
            }
            }
            break;
        }
        default:
            switch (node->kind)
            {
            case nodes::NodeKind::_LABEL:
            {
                auto label = (nodes::NodeLabel *)node->ptr.get();
                // check if this label already exists
                // if so then it is not allowed to redefine the same label twice
                // also check if the label is a procedure that was already defined
                auto res = symtable.find_entry(label->label_name);
                if (symtable.is_valid(res))
                {
                    if (res->second.type == symtable::SymEntryType::_PROC)
                    {
                        // it is a procedure
                        if (res->second.defined == true)
                        {
                            // the procedure was already defined
                            // cannot redefine
                            _l._register_sema_error_(node->line, "Redefinition of procedure " + res->first + " when it is already defined.", error::_PROC_REDFIN_);
                            _set_();
                            break;
                        }
                        else
                        {
                            // this is a procedure definition and it hasn't been defined before
                            res->second.defined = true;
                        }
                        break;
                    }
                    else
                    {
                        _l._register_sema_error_(node->line, "Redefinition of label " + res->first + " when it is already used.", error::_PROC_REDFIN_);
                        _set_();
                        break;
                    }
                    break;
                }
                else
                    symtable.add_entry(label->label_name, symtable::SymTableEntry(symtable::_LABEL));
                // since after the above conditional, the label will either be added to the symtable
                // or the assembling will halt due to error
                // so we can do the following
                if (label->label_name == "main")
                {
                    // we need to save the current pos
                    main_proc_index = inst_nodes.size();
                }
                break;
            }
            case nodes::NodeKind::_PROC_DECLR:
            {
                auto proc = (nodes::NodeProcDeclr *)node->ptr.get();
                // check if this proc already exists
                // if so then it is not allowed to redefine the same proc twice
                if (symtable.is_valid(symtable.find_entry(proc->proc_name)))
                {
                    _l._register_sema_error_(node->line, "Redeclaration of procedure " + proc->proc_name + " when it was previously defined.", error::_PROC_REDECLR_);
                    _set_();
                    break;
                }
                symtable.add_entry(proc->proc_name, symtable::SymTableEntry(symtable::_PROC));
                break;
            }
            }
            inst_nodes.push_back(std::move(node));
            break;
        }
    }
}

void zeta::sema::Sema::validate_symbols()
{
    for (auto &inst : inst_nodes)
    {
        // we only check for those instructions that may use variables and symbols
        switch (inst->kind)
        {
        case nodes::NodeKind::_INST_CMPXCHG:
        {
            auto node = (nodes::NodeCmpXchg *)inst->ptr.get();
            auto x = symtable.find_entry(node->var_name);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->var_name + "' in the cmpxchg instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_STORE:
        case nodes::NodeKind::_INST_STOREB:
        case nodes::NodeKind::_INST_STOREW:
        case nodes::NodeKind::_INST_STORED:
        case nodes::NodeKind::_INST_ATM_STORE:
        case nodes::NodeKind::_INST_ATM_STOREB:
        case nodes::NodeKind::_INST_ATM_STOREW:
        case nodes::NodeKind::_INST_ATM_STORED:
        {
            auto node = (nodes::NodeStore *)inst->ptr.get();
            auto x = symtable.find_entry(node->var_name);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->var_name + "' in the store instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_LOAD:
        case nodes::NodeKind::_INST_LOADB:
        case nodes::NodeKind::_INST_LOADW:
        case nodes::NodeKind::_INST_LOADD:
        case nodes::NodeKind::_INST_ATM_LOAD:
        case nodes::NodeKind::_INST_ATM_LOADB:
        case nodes::NodeKind::_INST_ATM_LOADW:
        case nodes::NodeKind::_INST_ATM_LOADD:
        {
            auto node = (nodes::NodeLoad *)inst->ptr.get();
            auto x = symtable.find_entry(node->var_name);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->var_name + "' in the load instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_MOVF:
        case nodes::NodeKind::_INST_MOVLF:
        {
            // these instructions are different in the sense that they both accept variables of type df and dlf
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the mov instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_FLOAT && x->second.dtype != nodes::DataType::_TYPE_LFLOAT)
                {
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of FLOAT type to be used with 'movF'", error::_TYPE_DIFF_);
                    _set_();
                    break;
                }
            }
            break;
        }
        case nodes::NodeKind::_INST_SIN:
        {
            auto node = (nodes::NodeOneImmOperand *)inst->ptr.get();
            auto x = symtable.find_entry(node->imm);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->imm + "' in the sin instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_SOUT:
        {
            auto node = (nodes::NodeOneImmOperand *)inst->ptr.get();
            auto x = symtable.find_entry(node->imm);
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->imm + "' in the sout instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMMQ:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the movq instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_QWORD && x->second.dtype != nodes::DataType::_TYPE_RESQ)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of QWORD type to be used with 'movq'", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMM8:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the movb instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_BYTE && x->second.dtype != nodes::DataType::_TYPE_STRING && x->second.dtype != nodes::DataType::_TYPE_RESB)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of BYTE type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMM16:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_WORD && x->second.dtype != nodes::DataType::_TYPE_RESW)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of WORD type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMM32:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_DWORD && x->second.dtype != nodes::DataType::_TYPE_RESD)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of DWORD type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM8:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_BYTE && x->second.dtype != nodes::DataType::_TYPE_RESB)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of BYTE type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM16:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_WORD && x->second.dtype != nodes::DataType::_TYPE_RESW)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of WORD type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOVSX_REG_IMM32:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype != nodes::DataType::_TYPE_DWORD && x->second.dtype != nodes::DataType::_TYPE_RESD)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of DWORD type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOV_REG_IMM:
        {
            auto node = (nodes::NodeInstMovRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                if (!symtable.is_valid(symtable.find_entry(node->value)))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the move instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
            }
            break;
        }
        case nodes::NodeKind::_INST_ADD_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the add instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_SUB_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the sub instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MUL_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the mul instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_DIV_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the div instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_MOD_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the mod instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_IADD_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the iadd instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_ISUB_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the isub instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_IMUL_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the imul instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_IDIV_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the idiv instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_IMOD_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            if (node->is_iden)
            {
                auto x = symtable.find_entry(node->value);
                if (!symtable.is_valid(x))
                {
                    _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the imod instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                    _set_();
                    break;
                }
                if (x->second.dtype == nodes::DataType::_TYPE_STRING || x->second.dtype == nodes::DataType::_TYPE_FLOAT || x->second.dtype == nodes::DataType::_TYPE_LFLOAT)
                    _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of NUM type as expected by the instruction.", error::_TYPE_MISMATCH_);
            }
            break;
        }
        case nodes::NodeKind::_INST_FADD_IMM:
        case nodes::NodeKind::_INST_LFADD_IMM:
        case nodes::NodeKind::_INST_FSUB_IMM:
        case nodes::NodeKind::_INST_LFSUB_IMM:
        case nodes::NodeKind::_INST_FMUL_IMM:
        case nodes::NodeKind::_INST_LFMUL_IMM:
        case nodes::NodeKind::_INST_FDIV_IMM:
        case nodes::NodeKind::_INST_LFDIV_IMM:
        {
            auto node = (nodes::NodeArithmeticRegImm *)inst->ptr.get();
            auto x = symtable.find_entry(node->value);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The operand '" + node->value + "' in the floating-point instruction is not a valid identifier.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            if (x->second.dtype != nodes::DataType::_TYPE_LFLOAT && x->second.dtype != nodes::DataType::_TYPE_FLOAT)
            {
                _l._register_sema_error_(inst->line, "The variable '" + x->first + "' is not of FLOAT type as expected by the instruction.", error::_TYPE_DIFF_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_JMP:
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
        {
            auto node = (nodes::NodeControlFlow *)inst->ptr.get();
            auto x = symtable.find_entry(node->_jmp_label_);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The label " + node->_jmp_label_ + " doesn't exist.", error::_LABEL_DOESNT_EXIST_);
                _set_();
                break;
            }
            if (x->second.type != symtable::SymEntryType::_LABEL && x->second.type != symtable::SymEntryType::_PROC)
            {
                _l._register_sema_error_(inst->line, "JMP to label " + node->_jmp_label_ + " is not a label at all.", error::_INVALID_JMP_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_LOOP:
        {
            auto node = (nodes::NodeControlFlow *)inst->ptr.get();
            auto x = symtable.find_entry(node->_jmp_label_);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The label " + node->_jmp_label_ + " doesn't exist.", error::_LABEL_DOESNT_EXIST_);
                _set_();
                break;
            }
            if (x->second.type != symtable::SymEntryType::_LABEL && x->second.type != symtable::SymEntryType::_PROC)
            {
                _l._register_sema_error_(inst->line, "LOOP to label " + node->_jmp_label_ + " is not a label at all.", error::_INVALID_JMP_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_CALL:
        {
            auto node = (nodes::NodeControlFlow *)inst->ptr.get();
            auto x = symtable.find_entry(node->_jmp_label_);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The procedure " + node->_jmp_label_ + " doesn't exist.", error::_PROC_DOESNT_EXIST_);
                _set_();
                break;
            }
            if (x->second.type != symtable::SymEntryType::_PROC)
            {
                _l._register_sema_error_(inst->line, "CALL to procedure " + node->_jmp_label_ + " is not a procedure at all.", error::_INVALID_JMP_);
                _set_();
                break;
            }
            break;
        }
        case nodes::NodeKind::_INST_CMP_IMM:
        {
            auto node = (nodes::NodeCmpImm *)inst->ptr.get();
            if (!node->is_iden)
                break;
            auto x = symtable.find_entry(node->val);
            if (!symtable.is_valid(x))
            {
                _l._register_sema_error_(inst->line, "The variable " + node->val + " doesn't exist.", error::_IDEN_DOESNT_EXIST_);
                _set_();
                break;
            }
            break;
        }
        }
    }
}

zeta::error::_STATES_ zeta::sema::Sema::analyse()
{
    gen_symtable();
    // check if all the procedures have been correctly called
    check_proc_declr();
    // Now check if the instructions have valid symbols or not
    validate_symbols();
    return state;
}
