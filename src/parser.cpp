#include "../includes/parser.hpp"

void zeta::parser::Parser::move_nodes(std::vector<std::unique_ptr<nodes::Node>> &dest)
{
    dest = std::move(nodes);
}

zeta::error::_STATES_ zeta::parser::Parser::parse()
{
    section = _SECTION_NONE;
    while (curr_tok.type != tokens::_TT_ERR && !err && curr_tok.type != tokens::_TT_EOF)
    {
        if (curr_tok.type == tokens::_TT_SECTION_DATA)
            parseDataSection();
        else if (curr_tok.type == tokens::_TT_SECTION_TEXT)
            parseTextSection();
        else if (curr_tok.type == tokens::_TT_IDENTIFIER)
        {
            handle_identifier();
            if (nodes.back()->kind != nodes::NodeKind::_LABEL)
                next_token();
        }
        else if (curr_tok.type == tokens::_TT_KEY_PROC)
        {
            if (section != _SECTION_TEXT)
            {
                lexer._register_parser_error_("Declaring a procedure in the data section.", "proc");
                set();
            }
            else
            {
                handle_proc_declaration();
                next_token();
            }
        }
        else if (curr_tok.type == tokens::_TT_KEY_ATM)
        {
            handle_atm_insts();
        }
        else if (curr_tok.type >= tokens::_TT_INST_NOP && curr_tok.type <= tokens::_TT_INST_DEC)
            handleInstruction();
        else
        {
            lexer._register_parser_error_("Expected an identifier, a keyword, number or anything useful.", curr_tok.value);
            set();
        }
    }
    if (!encountered_text)
    {
        std::cerr << "Parse Error: The input contains no text section which is not allowed." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (err)
        return error::_ERR_;
    return error::_NORMAL_;
}

void zeta::parser::Parser::parseTextSection()
{
    next_token();
    if (curr_tok.type != tokens::_TT_OPER_COLON)
    {
        lexer._register_parser_error_("Expected ':' after \'.text\' got " + curr_tok.value + " instead.", curr_tok.value);
        set();
    }
    section = _SECTION_TEXT;
    encountered_text = true;
    next_token();
}

void zeta::parser::Parser::parseDataSection()
{
    next_token();
    if (curr_tok.type != tokens::_TT_OPER_COLON)
    {
        lexer._register_parser_error_("Expected ':' after \'.data\' got " + curr_tok.value + " instead.", curr_tok.value);
        set();
    }
    section = _SECTION_DATA;
    next_token();
}

void zeta::parser::Parser::handle_identifier()
{
    std::string name = curr_tok.value;
    next_token();

    if (curr_tok.type != tokens::_TT_OPER_COLON)
    {
        lexer._register_parser_error_("Expected ':' after " + name + " got " + curr_tok.value + " instead.", curr_tok.value);
        set();
        return;
    }
    next_token();

    switch (curr_tok.type)
    {
    case tokens::_TT_KEY_DB:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_defines(name, nodes::_DEF_BYTE);
        break;
    }
    case tokens::_TT_KEY_DW:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_defines(name, nodes::_DEF_WORD);
        break;
    }
    case tokens::_TT_KEY_DD:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_defines(name, nodes::_DEF_DWORD);
        break;
    }
    case tokens::_TT_KEY_DQ:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_defines(name, nodes::_DEF_QWORD);
        break;
    }
    case tokens::_TT_KEY_DF:
    case tokens::_TT_KEY_DLF:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_definefloats(name);
        break;
    }
    case tokens::_TT_KEY_STRING:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_string(name);
        break;
    }
    case tokens::_TT_KEY_RESB:
    case tokens::_TT_KEY_RESW:
    case tokens::_TT_KEY_RESD:
    case tokens::_TT_KEY_RESQ:
    {
        if (section != _SECTION_DATA)
            goto _txt_error;
        handle_resX(name);
        break;
    }
    default:
    {
        if (section == _SECTION_DATA)
        {
            lexer._register_parser_error_("Defining labels in the variable section is not allowed", curr_tok.value);
            set();
            return;
        }
        // this is a label
        handle_label(name);
        break;
    }
    }
    return;
_txt_error:
    lexer._register_parser_error_("Defining variables in the text section is not allowed", curr_tok.value);
    set();
    return;
}

void zeta::parser::Parser::handle_proc_declaration()
{
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a procedure after 'proc' keyword.");
        return;
    }
    nodes::NodeKind kind;
    std::unique_ptr<nodes::Base> ptr;
    kind = nodes::_PROC_DECLR;
    ptr = std::make_unique<nodes::NodeProcDeclr>();
    auto temp = (nodes::NodeProcDeclr *)ptr.get();
    temp->proc_name = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_atm_insts()
{
    next_token();
    // handle atomic instructions here
    switch (curr_tok.type)
    {
    case tokens::_TT_INST_LOADB:
    case tokens::_TT_INST_LOADW:
    case tokens::_TT_INST_LOADD:
        handle_inst_loadX();
        break;
    case tokens::_TT_INST_STOREB:
    case tokens::_TT_INST_STOREW:
    case tokens::_TT_INST_STORED:
        handle_inst_storeX();
        break;
    case tokens::_TT_INST_STORE:
        handle_inst_store();
        break;
    case tokens::_TT_INST_LOAD:
        handle_inst_load();
        break;
    default:
        send_errors("This is not a valid atomic operation.");
        return;
    }
    // now we need to change the node type
    if (err)
        return;
    auto temp = nodes[nodes.size() - 1].get();
    if (temp->kind >= 150 && temp->kind <= 157)
    {
        curr_tok.value = "";
        send_errors("Atomic load and store requires one operand to be a variable.");
    }
    temp->kind = (nodes::NodeKind)((uint64_t)temp->kind + 16);
    next_token();
}

void zeta::parser::Parser::handleInstruction()
{
    if (section != _SECTION_TEXT)
    {
        send_errors("Using instructions in the data section is not allowed");
    }
    switch (curr_tok.type)
    {
    case tokens::_TT_INST_NOP:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::Node(nodes::_TYPE_INST, nodes::_INST_NOP, std::make_unique<nodes::Base>())));
        break;
    case tokens::_TT_INST_HLT:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::Node(nodes::_TYPE_INST, nodes::_INST_HLT, std::make_unique<nodes::Base>())));
        break;
    case tokens::_TT_INST_MOVQ:
    case tokens::_TT_INST_MOV:
        handle_inst_mov();
        break;
    case tokens::_TT_INST_MOVB:
    case tokens::_TT_INST_MOVW:
    case tokens::_TT_INST_MOVD:
        handle_inst_movX();
        break;
    case tokens::_TT_INST_MOVEB:
    case tokens::_TT_INST_MOVEW:
    case tokens::_TT_INST_MOVED:
        handle_inst_moveX();
        break;
    case tokens::_TT_INST_MOVSXB:
    case tokens::_TT_INST_MOVSXW:
    case tokens::_TT_INST_MOVSXD:
        handle_inst_movsx();
        break;
    case tokens::_TT_INST_OUTR:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, nodes::NodeKind::_INST_OUTR, std::move(std::make_unique<nodes::Base>())));
        break;
    case tokens::_TT_INST_UOUTR:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, nodes::NodeKind::_INST_UOUTR, std::move(std::make_unique<nodes::Base>())));
        break;
    case tokens::_TT_INST_CIN:
        handle_inst_Xin(nodes::NodeKind::_INST_CIN);
        break;
    case tokens::_TT_INST_SIN:
        handle_inst_sin();
        break;
    case tokens::_TT_INST_IN:
        handle_inst_Xin(nodes::NodeKind::_INST_IN);
        break;
    case tokens::_TT_INST_COUT:
        handle_inst_Xout(nodes::NodeKind::_INST_COUT);
        break;
    case tokens::_TT_INST_OUT:
        handle_inst_Xout(nodes::NodeKind::_INST_OUT);
        break;
    case tokens::_TT_INST_SOUT:
        handle_inst_sout();
        break;
    case tokens::_TT_INST_INW:
        handle_inst_Xin(nodes::NodeKind::_INST_INW);
        break;
    case tokens::_TT_INST_OUTW:
        handle_inst_Xout(nodes::NodeKind::_INST_OUTW);
        break;
    case tokens::_TT_INST_IND:
        handle_inst_Xin(nodes::NodeKind::_INST_IND);
        break;
    case tokens::_TT_INST_OUTD:
        handle_inst_Xout(nodes::NodeKind::_INST_OUTD);
        break;
    case tokens::_TT_INST_INQ:
        handle_inst_Xin(nodes::NodeKind::_INST_INQ);
        break;
    case tokens::_TT_INST_OUTQ:
        handle_inst_Xout(nodes::NodeKind::_INST_OUTQ);
        break;
    case tokens::_TT_INST_UIN:
        handle_inst_Xin(nodes::NodeKind::_INST_UIN);
        break;
    case tokens::_TT_INST_UOUT:
        handle_inst_Xout(nodes::NodeKind::_INST_UOUT);
        break;
    case tokens::_TT_INST_UINW:
        handle_inst_Xin(nodes::NodeKind::_INST_UINW);
        break;
    case tokens::_TT_INST_UOUTW:
        handle_inst_Xout(nodes::NodeKind::_INST_UOUTW);
        break;
    case tokens::_TT_INST_UIND:
        handle_inst_Xin(nodes::NodeKind::_INST_UIND);
        break;
    case tokens::_TT_INST_UOUTD:
        handle_inst_Xout(nodes::NodeKind::_INST_UOUTD);
        break;
    case tokens::_TT_INST_UINQ:
        handle_inst_Xin(nodes::NodeKind::_INST_UINQ);
        break;
    case tokens::_TT_INST_UOUTQ:
        handle_inst_Xout(nodes::NodeKind::_INST_UOUTQ);
        break;
    case tokens::_TT_INST_INF:
        handle_inst_Xout(nodes::NodeKind::_INST_INF);
        break;
    case tokens::_TT_INST_INLF:
        handle_inst_Xout(nodes::NodeKind::_INST_INLF);
        break;
    case tokens::_TT_INST_OUTF:
        handle_inst_Xout(nodes::NodeKind::_INST_OUTF);
        break;
    case tokens::_TT_INST_OUTLF:
        handle_inst_Xout(nodes::NodeKind::_INST_OUTLF);
        break;
    case tokens::_TT_INST_MOVF:
    case tokens::_TT_INST_MOVLF:
        handle_inst_movf();
        break;
    case tokens::_TT_INST_ADD:
    case tokens::_TT_INST_IADD:
    case tokens::_TT_INST_SUB:
    case tokens::_TT_INST_ISUB:
    case tokens::_TT_INST_DIV:
    case tokens::_TT_INST_IDIV:
    case tokens::_TT_INST_MUL:
    case tokens::_TT_INST_IMUL:
    case tokens::_TT_INST_MOD:
    case tokens::_TT_INST_IMOD:
        handle_inst_arithmetic();
        break;
    case tokens::_TT_INST_FADD:
    case tokens::_TT_INST_LFADD:
    case tokens::_TT_INST_FSUB:
    case tokens::_TT_INST_LFSUB:
    case tokens::_TT_INST_FMUL:
    case tokens::_TT_INST_LFMUL:
    case tokens::_TT_INST_FDIV:
    case tokens::_TT_INST_LFDIV:
        handle_inst_farithmetic();
        break;
    case tokens::_TT_INST_JMP:
        handle_inst_jmpcall(nodes::_INST_JMP);
        break;
    case tokens::_TT_INST_SET_EXCP:
        handle_inst_jmpcall(nodes::_INST_SET_EXCP);
        break;
    case tokens::_TT_INST_CALL_EXCP:
        handle_inst_jmpcall(nodes::_INST_CALL_EXCP);
        break;
    case tokens::_TT_INST_CMP:
        handle_inst_cmp();
        break;
    case tokens::_TT_INST_JNZ:
    case tokens::_TT_INST_JZ:
    case tokens::_TT_INST_JNE:
    case tokens::_TT_INST_JE:
    case tokens::_TT_INST_JNC:
    case tokens::_TT_INST_JC:
    case tokens::_TT_INST_JNO:
    case tokens::_TT_INST_JO:
    case tokens::_TT_INST_JNN:
    case tokens::_TT_INST_JN:
    case tokens::_TT_INST_JNG:
    case tokens::_TT_INST_JG:
    case tokens::_TT_INST_JNS:
    case tokens::_TT_INST_JS:
    case tokens::_TT_INST_JGE:
    case tokens::_TT_INST_JSE:
        handle_inst_jX();
        break;
    case tokens::_TT_INST_INC:
        handle_inst_incdec("inc", nodes::_INST_INC);
        break;
    case tokens::_TT_INST_DEC:
        handle_inst_incdec("dec", nodes::_INST_DEC);
        break;
    case tokens::_TT_INST_AND:
    case tokens::_TT_INST_OR:
    case tokens::_TT_INST_XOR:
        handle_inst_logical();
        break;
    case tokens::_TT_INST_NOT:
        handle_inst_incdec("not", nodes::_INST_NOT);
        break;
    case tokens::_TT_INST_LSHIFT:
        handle_inst_shift("lshift", nodes::_INST_LSHIFT);
        break;
    case tokens::_TT_INST_RSHIFT:
        handle_inst_shift("rshift", nodes::_INST_RSHIFT);
        break;
    case tokens::_TT_INST_CFLAGS:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CFLAGS, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_RESET:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_RESET, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_CLC:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CLC, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_CLN:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CLN, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_CLO:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CLO, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_CLZ:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CLZ, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_RET:
        nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_RET, std::make_unique<nodes::Base>()));
        break;
    case tokens::_TT_INST_CALL:
        handle_inst_jmpcall(nodes::_INST_CALL);
        break;
    case tokens::_TT_INST_SVA:
    case tokens::_TT_INST_SVC:
        handle_inst_sva_svc();
        break;
    case tokens::_TT_INST_PUSH:
    case tokens::_TT_INST_PUSHA:
        handle_inst_push();
        break;
    case tokens::_TT_INST_POP:
    case tokens::_TT_INST_POPA:
        handle_inst_pop();
        break;
    case tokens::_TT_INST_LEA:
        handle_inst_lea();
        break;
    case tokens::_TT_INST_STORE:
        handle_inst_store();
        break;
    case tokens::_TT_INST_LOAD:
        handle_inst_load();
        break;
    case tokens::_TT_INST_EXCG:
        handle_inst_excg();
        break;
    case tokens::_TT_INST_EXCG8:
    case tokens::_TT_INST_EXCG16:
    case tokens::_TT_INST_EXCG32:
        handle_inst_excgX();
        break;
    case tokens::_TT_INST_LOOP:
        handle_inst_jmpcall(nodes::_INST_LOOP);
        break;
    case tokens::_TT_INST_INTR:
        handle_inst_intr();
        break;
    case tokens::_TT_INST_LOADB:
    case tokens::_TT_INST_LOADW:
    case tokens::_TT_INST_LOADD:
        handle_inst_loadX();
        break;
    case tokens::_TT_INST_STOREB:
    case tokens::_TT_INST_STOREW:
    case tokens::_TT_INST_STORED:
        handle_inst_storeX();
        break;
    case tokens::_TT_INST_CMPXCHG:
        handle_inst_cmpxchg();
        break;
    }
    if (!err)
        next_token();
}

void zeta::parser::Parser::handle_defines(std::string name, nodes::NodeKind k)
{
    next_token();
    if (curr_tok.type != tokens::_TT_INT && curr_tok.type != tokens::_TT_NINT)
    {
        send_errors("Expected a number, got " + curr_tok.value + " instead.");
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeDefs>();
    auto temp = (nodes::NodeDefs *)ptr.get();
    temp->byte_name = name;
    temp->byte_val = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_DATA, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_definefloats(std::string name)
{
    nodes::NodeKind kind = curr_tok.type == tokens::_TT_KEY_DF ? nodes::_DEF_FLOAT : nodes::_DEF_LFLOAT;
    next_token();
    if (curr_tok.type != tokens::_TT_FLOAT && curr_tok.type != tokens::_TT_NFLOAT)
    {
        send_errors("Expected a float, got " + curr_tok.value + " instead.");
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeDefs>();
    auto temp = (nodes::NodeDefs *)(ptr.get());
    temp->byte_name = name;
    temp->byte_val = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_DATA, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_string(std::string name)
{
    next_token();
    if (curr_tok.type != tokens::_TT_STRING)
    {
        send_errors("Expected a string, got " + curr_tok.value + " instead.");
        return;
    }
    nodes::NodeKind kind;
    std::unique_ptr<nodes::Base> ptr;
    kind = nodes::_DEF_STRING;
    ptr = std::make_unique<nodes::NodeDefs>();
    auto temp = (nodes::NodeDefs *)(ptr.get());
    temp->byte_name = name;
    temp->byte_val = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_DATA, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_resX(std::string name)
{
    auto kind = curr_tok.type == tokens::_TT_KEY_RESB ? nodes::_DEF_RESB : curr_tok.type == tokens::_TT_KEY_RESW ? nodes::_DEF_RESW
                                                                       : curr_tok.type == tokens::_TT_KEY_RESD   ? nodes::_DEF_RESD
                                                                                                                 : nodes::_DEF_RESQ;
    next_token();
    if (curr_tok.type != tokens::_TT_INT)
    {
        send_errors("Expected a positive number after 'resX'.");
        return;
    }
    if (curr_tok.value == "0")
    {
        send_errors("Cannot reserve 0 bytes");
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeRes>();
    auto temp = (nodes::NodeRes *)(ptr.get());
    temp->number = std::stoull(curr_tok.value);
    temp->name = name;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_DATA, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_label(std::string label_name)
{
    nodes::NodeKind kind;
    std::unique_ptr<nodes::Base> ptr;
    kind = nodes::_LABEL;
    ptr = std::make_unique<nodes::NodeLabel>();
    auto temp = (nodes::NodeLabel *)ptr.get();
    temp->label_name = label_name;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_loadX()
{
    /*
      In the case of loadX and storeX instructions, the assembler will not complain
      if the variable is of a different size then the instruction can handle.
      The behaviour may be unexpected.
    */
    auto x = curr_tok.type;
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    nodes::Registers src;
    nodes::NodeKind k;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the source.");
        return;
    }
    src = regr->second;
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a label after to load the source.");
        return;
    }
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr != nodes::_regr_iden_map.end())
    {
        k = x == tokens::_TT_INST_LOADB ? nodes::_INST_LOAD_REG : tokens::_TT_INST_LOADW ? nodes::_INST_LOADW_REG
                                                                                         : nodes::_INST_LOADD_REG;
        ptr = std::make_unique<nodes::NodeLoadReg>();
        auto temp = (nodes::NodeLoadReg *)ptr.get();
        temp->dest = src;
        temp->addr_regr = regr->second;
    }
    else
    {
        k = x == tokens::_TT_INST_LOADB ? nodes::_INST_LOADB : tokens::_TT_INST_LOADW ? nodes::_INST_LOADW
                                                                                      : nodes::_INST_LOADD;
        ptr = std::make_unique<nodes::NodeLoad>();
        auto temp = (nodes::NodeLoad *)ptr.get();
        temp->dest = src;
        temp->var_name = curr_tok.value;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_storeX()
{
    auto x = curr_tok.type;
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    nodes::Registers src;
    nodes::NodeKind k;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the source.");
        return;
    }
    src = regr->second;
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a label after to store the source.");
        return;
    }
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr != nodes::_regr_iden_map.end())
    {
        k = x == tokens::_TT_INST_STOREB ? nodes::_INST_STOREB_REG : x == tokens::_TT_INST_STORED ? nodes::_INST_STORED_REG
                                                                                                  : nodes::_INST_STOREW_REG;
        ptr = std::make_unique<nodes::NodeStoreReg>();
        auto temp = (nodes::NodeStoreReg *)ptr.get();
        temp->dest = src;
        temp->addr_regr = regr->second;
    }
    else
    {
        k = x == tokens::_TT_INST_STOREB ? nodes::_INST_STOREB : x == tokens::_TT_INST_STORED ? nodes::_INST_STORED
                                                                                              : nodes::_INST_STOREW;
        ptr = std::make_unique<nodes::NodeStore>();
        auto temp = (nodes::NodeStore *)ptr.get();
        temp->dest = src;
        temp->var_name = curr_tok.value;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_load()
{
    // exactly same method as store
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    nodes::Registers src;
    nodes::NodeKind k;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the source.");
        return;
    }
    src = regr->second;
    next_token();
    // now we need a variable name or a register
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a label after to store the source.");
        return;
    }
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr != nodes::_regr_iden_map.end())
    {
        k = nodes::_INST_LOAD_REG;
        ptr = std::make_unique<nodes::NodeLoadReg>();
        auto temp = (nodes::NodeLoadReg *)ptr.get();
        temp->dest = src;
        temp->addr_regr = regr->second;
    }
    else
    {
        k = nodes::_INST_LOAD;
        ptr = std::make_unique<nodes::NodeLoad>();
        auto temp = (nodes::NodeLoad *)ptr.get();
        temp->dest = src;
        temp->var_name = curr_tok.value;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_store()
{
    // store will store whatever is in the operand register into the given memory address
    next_token();
    // we need a register here
    std::unique_ptr<nodes::Base> ptr;
    nodes::Registers src;
    nodes::NodeKind k;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the source.");
        return;
    }
    src = regr->second;
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a label after to store the source.");
        return;
    }
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr != nodes::_regr_iden_map.end())
    {
        k = nodes::_INST_STORE_REG;
        ptr = std::make_unique<nodes::NodeStoreReg>();
        auto temp = (nodes::NodeStoreReg *)ptr.get();
        temp->dest = src;
        temp->addr_regr = regr->second;
    }
    else
    {
        k = nodes::_INST_STORE;
        ptr = std::make_unique<nodes::NodeStore>();
        auto temp = (nodes::NodeStore *)ptr.get();
        temp->dest = src;
        temp->var_name = curr_tok.value;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_mov()
{
    // mov and movq
    // possible variants:
    // <> reg, reg | <> reg, imm | <> reg, variable
    // We have to identify what the variant is
    bool is_q = (curr_tok.type == tokens::_TT_INST_MOVQ);
    next_token();
    // This must be a register no matter what
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected register name after 'mov' instruction.");
        return;
    }
    next_token();
    nodes::NodeKind kind;
    std::unique_ptr<nodes::Base> ptr;
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        // then it could be a mov reg, reg
        // or it could be a variable as well
        auto iden2 = nodes::_regr_iden_map.find(curr_tok.value);
        if (iden2 == nodes::_regr_iden_map.end())
        {
            // it is a variable name
            kind = nodes::NodeKind::_INST_MOV_REG_IMM;
            ptr = std::make_unique<nodes::NodeInstMovRegImm>();
            auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
            temp->is_iden = true;
            temp->dest_regr = regr->second;
            temp->value = curr_tok.value;
        }
        else
        {
            kind = nodes::NodeKind::_INST_MOV_REG_REG;
            ptr = std::make_unique<nodes::NodeInstMovRegReg>();
            auto temp = (nodes::NodeInstMovRegReg *)ptr.get();
            temp->src_reg = iden2->second;
            temp->dest_regr = regr->second;
            is_q = false;
        }
    }
    else if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
    {
        // then we have an immediate here
        // in future based on the size of the number, we could encode mov64 instruction
        kind = nodes::NodeKind::_INST_MOV_REG_IMM;
        ptr = std::make_unique<nodes::NodeInstMovRegImm>();
        auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
        temp->dest_regr = regr->second;
        temp->value = curr_tok.value;
    }
    else
    {
        send_errors("Expected an immediate value, register or a variable name.");
        return;
    }
    kind = is_q ? nodes::NodeKind::_INST_MOV_REG_IMMQ : kind;
    nodes.push_back(std::make_unique<zeta::nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_movX()
{
    // just mov instruction but for smaller data types
    // movb, movw, movd
    // movq is handled above
    size_t len = curr_tok.type == tokens::_TT_INST_MOVB ? 1 : curr_tok.type == tokens::_TT_INST_MOVW ? 2
                                                                                                     : 4;
    next_token();
    nodes::NodeKind kind;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected register name after 'movX' instruction.");
        return;
    }
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto iden2 = nodes::_regr_iden_map.find(curr_tok.value);
        if (iden2 == nodes::_regr_iden_map.end())
        {
            // it is a variable name
            kind = len == 1 ? nodes::NodeKind::_INST_MOV_REG_IMM8 : len == 2 ? nodes::NodeKind::_INST_MOV_REG_IMM16
                                                                             : nodes::NodeKind::_INST_MOV_REG_IMM32;
            ptr = std::make_unique<nodes::NodeInstMovRegImm>();
            auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
            temp->is_iden = true;
            temp->dest_regr = regr->second;
            temp->value = curr_tok.value;
        }
        else
        {
            kind = len == 1 ? nodes::NodeKind::_INST_MOV_REG_REG8 : len == 2 ? nodes::NodeKind::_INST_MOV_REG_REG16
                                                                             : nodes::NodeKind::_INST_MOV_REG_REG32;
            ptr = std::make_unique<nodes::NodeInstMovRegReg>();
            auto temp = (nodes::NodeInstMovRegReg *)ptr.get();
            temp->src_reg = iden2->second;
            temp->dest_regr = regr->second;
        }
    }
    else if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
    {
        // then we have an immediate here
        // in future based on the size of the number, we could encode mov64 instruction
        kind = len == 1 ? nodes::NodeKind::_INST_MOV_REG_IMM8 : len == 2 ? nodes::NodeKind::_INST_MOV_REG_IMM16
                                                                         : nodes::NodeKind::_INST_MOV_REG_IMM32;
        ptr = std::make_unique<nodes::NodeInstMovRegImm>();
        auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
        temp->dest_regr = regr->second;
        temp->value = curr_tok.value;
    }
    else
    {
        send_errors("Expected an immediate value, register or a variable name.");
        return;
    }
    nodes.push_back(std::make_unique<zeta::nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_moveX()
{
    // moveb, movew, moved are just mov instruction but they only take registers as their operands
    size_t len = curr_tok.type == tokens::_TT_INST_MOVEB ? 1 : curr_tok.type == tokens::_TT_INST_MOVEW ? 2
                                                                                                       : 4;
    next_token();
    nodes::NodeKind kind;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected register name after 'movX' instruction.");
        return;
    }
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto iden2 = nodes::_regr_iden_map.find(curr_tok.value);
        if (iden2 == nodes::_regr_iden_map.end())
        {
            send_errors("The moveX instruction doesn't take a variable as it's operand.");
            return;
        }
        else
        {
            kind = len == 1 ? nodes::NodeKind::_INST_MOV_REG_MOVEB : len == 2 ? nodes::NodeKind::_INST_MOV_REG_MOVEW
                                                                              : nodes::NodeKind::_INST_MOV_REG_MOVED;
            ptr = std::make_unique<nodes::NodeInstMovRegReg>();
            auto temp = (nodes::NodeInstMovRegReg *)ptr.get();
            temp->src_reg = iden2->second;
            temp->dest_regr = regr->second;
        }
    }
    else if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
    {
        send_errors("The moveX instruction expects registers as it's only operands not immediates.");
        return;
    }
    else
    {
        send_errors("Expected a register here.");
        return;
    }
    nodes.push_back(std::make_unique<zeta::nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_movsx()
{
    // movsxX sign extends source operand and puts it into destination register,
    // why not movsxq? Because it doesn't make sense
    size_t len = curr_tok.type == tokens::_TT_INST_MOVSXB ? 1 : curr_tok.type == tokens::_TT_INST_MOVSXW ? 2
                                                                                                         : 4;
    next_token();
    nodes::NodeKind kind;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected register name after 'movsxX' instruction.");
        return;
    }
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto iden2 = nodes::_regr_iden_map.find(curr_tok.value);
        if (iden2 == nodes::_regr_iden_map.end())
        {
            kind = len == 1 ? nodes::NodeKind::_INST_MOVSX_REG_IMM8 : len == 2 ? nodes::NodeKind::_INST_MOVSX_REG_IMM16
                                                                               : nodes::NodeKind::_INST_MOVSX_REG_IMM32;
            ptr = std::make_unique<nodes::NodeInstMovRegImm>();
            auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
            temp->is_iden = true;
            temp->dest_regr = regr->second;
            temp->value = curr_tok.value;
        }
        else
        {
            kind = len == 1 ? nodes::NodeKind::_INST_MOVSX_REG_REG8 : len == 2 ? nodes::NodeKind::_INST_MOVSX_REG_REG16
                                                                               : nodes::NodeKind::_INST_MOVSX_REG_REG32;
            ptr = std::make_unique<nodes::NodeInstMovRegReg>();
            auto temp = (nodes::NodeInstMovRegReg *)ptr.get();
            temp->src_reg = iden2->second;
            temp->dest_regr = regr->second;
        }
    }
    else if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
    {
        // then we have an immediate here
        // in future based on the size of the number, we could encode mov64 instruction
        kind = len == 1 ? nodes::NodeKind::_INST_MOVSX_REG_IMM8 : len == 2 ? nodes::NodeKind::_INST_MOVSX_REG_IMM16
                                                                           : nodes::NodeKind::_INST_MOVSX_REG_IMM32;
        ptr = std::make_unique<nodes::NodeInstMovRegImm>();
        auto temp = (nodes::NodeInstMovRegImm *)ptr.get();
        temp->dest_regr = regr->second;
        temp->value = curr_tok.value;
    }
    else
    {
        send_errors("Expected an immediate value, register or a variable name.");
        return;
    }
    nodes.push_back(std::make_unique<zeta::nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_Xin(nodes::NodeKind k)
{
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a register after " + std::string(((k == nodes::_INST_INF || k == nodes::_INST_INLF) ? "inF." : "Xin.")));
        return;
    }
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register after " + std::string((k == nodes::_INST_INF || k == nodes::_INST_INLF) ? "inF." : "Xin."));
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeOneRegrOperands>();
    ((nodes::NodeOneRegrOperands *)(ptr.get()))->oper_rger = regr->second;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_sin()
{
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected an identifier after 'sin'.");
        return;
    }
    if (nodes::_regr_iden_map.find(curr_tok.value) != nodes::_regr_iden_map.end())
    {
        send_errors("The 'sin' instruction doesn't take registers as operands.");
        return;
    }
    nodes::NodeKind kind = nodes::NodeKind::_INST_SIN;
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeOneImmOperand>();
    auto temp = (nodes::NodeOneImmOperand *)ptr.get();
    temp->imm = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_Xout(nodes::NodeKind k)
{
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a register after " + std::string((k == nodes::_INST_OUTF || k == nodes::_INST_OUTLF) ? "outF." : "Xout."));
        return;
    }
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register after " + std::string((k == nodes::_INST_OUTF || k == nodes::_INST_OUTLF) ? "outF." : "Xout."));
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeOneRegrOperands>();
    ((nodes::NodeOneRegrOperands *)(ptr.get()))->oper_rger = regr->second;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_sout()
{
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected an identifier after 'sout'.");
        return;
    }
    if (nodes::_regr_iden_map.find(curr_tok.value) != nodes::_regr_iden_map.end())
    {
        send_errors("The 'sout' instruction doesn't take registers as operands.");
        return;
    }
    nodes::NodeKind kind = nodes::NodeKind::_INST_SOUT;
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeOneImmOperand>();
    auto temp = (nodes::NodeOneImmOperand *)ptr.get();
    temp->imm = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::NodeType::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_movf()
{
    nodes::NodeKind kind = curr_tok.type == tokens::_TT_INST_MOVF ? nodes::_INST_MOVF : nodes::_INST_MOVLF;
    next_token();
    auto dest_regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (dest_regr == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a destination register is the movF instruction.");
        return;
    }
    next_token();
    auto src_oper = nodes::_regr_iden_map.find(curr_tok.value);
    std::unique_ptr<nodes::Base> node = std::make_unique<nodes::NodeInstMovRegImm>();
    auto temp = (nodes::NodeInstMovRegImm *)node.get();
    temp->dest_regr = dest_regr->second;
    if (src_oper == nodes::_regr_iden_map.end())
    {
        // this is not a register
        // check if it is a number or a variable
        if (curr_tok.type == tokens::_TT_FLOAT || curr_tok.type == tokens::_TT_NFLOAT)
            temp->value = curr_tok.value;
        else if (curr_tok.type == tokens::_TT_IDENTIFIER)
        {
            temp->is_iden = true;
            temp->value = curr_tok.value;
        }
        else
        {
            send_errors("Expected a variable name or a floating point number.");
            return;
        }
    }
    else
    {
        // this instruction doesn't expect a register
        // there is no particular reason for this but still I DO IT BECAUSE I CAN
        send_errors("The floating point move instructions do not accept registers as the source.");
        return;
    }
    nodes.push_back(std::make_unique<zeta::nodes::Node>(nodes::_TYPE_INST, kind, std::move(node), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_arithmetic()
{
    nodes::NodeKind kind;
    auto temp_curr = curr_tok;
    std::unique_ptr<nodes::Base> ptr;
    next_token(); // goto the next token
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        // must be a register
        auto dest_regr = nodes::_regr_iden_map.find(curr_tok.value);
        if (dest_regr == nodes::_regr_iden_map.end())
        {
            send_errors("Expected a destination register.");
            return;
        }
        next_token();
        if (curr_tok.type != tokens::_TT_IDENTIFIER && curr_tok.type != tokens::_TT_INT && curr_tok.type != tokens::_TT_NINT)
        {
            send_errors("Expected a source register, immediate or a source register.");
            return;
        }
        auto src = nodes::_regr_iden_map.find(curr_tok.value);
        if (src == nodes::_regr_iden_map.end())
        {
            ptr = std::make_unique<nodes::NodeArithmeticRegImm>();
            auto temp = (nodes::NodeArithmeticRegImm *)ptr.get();
            temp->dest_regr = dest_regr->second;
            if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
                temp->is_iden = false;
            else
                temp->is_iden = true;
            temp->value = curr_tok.value;
            switch (temp_curr.type)
            {
            case tokens::_TT_INST_ADD:
            case tokens::_TT_INST_IADD:
                kind = (temp_curr.type == tokens::_TT_INST_ADD) ? nodes::_INST_ADD_IMM : nodes::_INST_IADD_IMM;
                break;
            case tokens::_TT_INST_SUB:
            case tokens::_TT_INST_ISUB:
                kind = (temp_curr.type == tokens::_TT_INST_SUB) ? nodes::_INST_SUB_IMM : nodes::_INST_ISUB_IMM;
                break;
            case tokens::_TT_INST_MUL:
            case tokens::_TT_INST_IMUL:
                kind = (temp_curr.type == tokens::_TT_INST_MUL) ? nodes::_INST_MUL_IMM : nodes::_INST_IMUL_IMM;
                break;
            case tokens::_TT_INST_DIV:
            case tokens::_TT_INST_IDIV:
                kind = (temp_curr.type == tokens::_TT_INST_DIV) ? nodes::_INST_DIV_IMM : nodes::_INST_IDIV_IMM;
                break;
            case tokens::_TT_INST_MOD:
            case tokens::_TT_INST_IMOD:
                kind = (temp_curr.type == tokens::_TT_INST_MOD) ? nodes::_INST_MOD_IMM : nodes::_INST_IMOD_IMM;
                break;
            }
        }
        else
        {
            ptr = std::make_unique<nodes::NodeArithmeticRegReg>();
            auto temp = (nodes::NodeArithmeticRegReg *)ptr.get();
            temp->dest_regr = dest_regr->second;
            temp->src_reg = src->second;
            switch (temp_curr.type)
            {
            case tokens::_TT_INST_ADD:
            case tokens::_TT_INST_IADD:
                kind = (temp_curr.type == tokens::_TT_INST_ADD) ? nodes::_INST_ADD_REG : nodes::_INST_IADD_REG;
                break;
            case tokens::_TT_INST_SUB:
            case tokens::_TT_INST_ISUB:
                kind = (temp_curr.type == tokens::_TT_INST_SUB) ? nodes::_INST_SUB_REG : nodes::_INST_ISUB_REG;
                break;
            case tokens::_TT_INST_MUL:
            case tokens::_TT_INST_IMUL:
                kind = (temp_curr.type == tokens::_TT_INST_MUL) ? nodes::_INST_MUL_REG : nodes::_INST_IMUL_REG;
                break;
            case tokens::_TT_INST_DIV:
            case tokens::_TT_INST_IDIV:
                kind = (temp_curr.type == tokens::_TT_INST_DIV) ? nodes::_INST_DIV_REG : nodes::_INST_IDIV_REG;
                break;
            case tokens::_TT_INST_MOD:
            case tokens::_TT_INST_IMOD:
                kind = (temp_curr.type == tokens::_TT_INST_MOD) ? nodes::_INST_MOD_REG : nodes::_INST_IMOD_REG;
                break;
            }
        }
    }
    else
    {
        send_errors("Expected a destination register.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_farithmetic()
{
    nodes::NodeKind kind;
    auto temp_curr = curr_tok;
    std::unique_ptr<nodes::Base> ptr;
    next_token();
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        // must be a register
        auto dest_regr = nodes::_regr_iden_map.find(curr_tok.value);
        if (dest_regr == nodes::_regr_iden_map.end())
        {
            send_errors("Expected a destination register.");
            return;
        }
        next_token();
        if (curr_tok.type != tokens::_TT_IDENTIFIER)
        {
            send_errors("Expected a source register or a variable.");
            return;
        }
        auto src = nodes::_regr_iden_map.find(curr_tok.value);
        if (src == nodes::_regr_iden_map.end())
        {
            ptr = std::make_unique<nodes::NodeArithmeticRegImm>();
            auto temp = (nodes::NodeArithmeticRegImm *)ptr.get();
            temp->dest_regr = dest_regr->second;
            temp->is_iden = true;
            temp->value = curr_tok.value;
            switch (temp_curr.type)
            {
            case tokens::_TT_INST_FADD:
            case tokens::_TT_INST_LFADD:
                kind = (temp_curr.type == tokens::_TT_INST_FADD) ? nodes::_INST_FADD_IMM : nodes::_INST_LFADD_IMM;
                break;
            case tokens::_TT_INST_FSUB:
            case tokens::_TT_INST_LFSUB:
                kind = (temp_curr.type == tokens::_TT_INST_FSUB) ? nodes::_INST_FSUB_IMM : nodes::_INST_LFSUB_IMM;
                break;
            case tokens::_TT_INST_FMUL:
            case tokens::_TT_INST_LFMUL:
                kind = (temp_curr.type == tokens::_TT_INST_FMUL) ? nodes::_INST_FMUL_IMM : nodes::_INST_LFMUL_IMM;
                break;
            case tokens::_TT_INST_FDIV:
            case tokens::_TT_INST_LFDIV:
                kind = (temp_curr.type == tokens::_TT_INST_FDIV) ? nodes::_INST_FDIV_IMM : nodes::_INST_LFDIV_IMM;
                break;
            }
        }
        else
        {
            ptr = std::make_unique<nodes::NodeArithmeticRegReg>();
            auto temp = (nodes::NodeArithmeticRegReg *)ptr.get();
            temp->dest_regr = dest_regr->second;
            temp->src_reg = src->second;
            switch (temp_curr.type)
            {
            case tokens::_TT_INST_FADD:
            case tokens::_TT_INST_LFADD:
                kind = (temp_curr.type == tokens::_TT_INST_FADD) ? nodes::_INST_FADD_REG : nodes::_INST_LFADD_REG;
                break;
            case tokens::_TT_INST_FSUB:
            case tokens::_TT_INST_LFSUB:
                kind = (temp_curr.type == tokens::_TT_INST_FSUB) ? nodes::_INST_FSUB_REG : nodes::_INST_LFSUB_REG;
                break;
            case tokens::_TT_INST_FMUL:
            case tokens::_TT_INST_LFMUL:
                kind = (temp_curr.type == tokens::_TT_INST_FMUL) ? nodes::_INST_FMUL_REG : nodes::_INST_LFMUL_REG;
                break;
            case tokens::_TT_INST_FDIV:
            case tokens::_TT_INST_LFDIV:
                kind = (temp_curr.type == tokens::_TT_INST_FDIV) ? nodes::_INST_FDIV_REG : nodes::_INST_LFDIV_REG;
                break;
            }
        }
    }
    else
    {
        send_errors("Expected a destination register.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, kind, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_jmpcall(nodes::NodeKind k)
{
    // handle the jump instruction here
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        // we expected an identifier as a label
        send_errors("Expected a label after the control flow instruction.");
        return;
    }
    // we also need to make sure that the ID is not a register
    if (nodes::_regr_iden_map.find(curr_tok.value) != nodes::_regr_iden_map.end())
    {
        // we don't need registers either
        send_errors("Expected a label as operand, not register.");
        return;
    }
    // we have everything needed
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeControlFlow>();
    auto temp = (nodes::NodeControlFlow *)ptr.get();
    temp->_jmp_label_ = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_cmp()
{
    next_token();
    // we expect a register as the first operand no matter what
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Compare instruction expects a register as it's first operand. Unknown operand.");
        return;
    }
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if (regr == nodes::_regr_iden_map.end())
    {
        send_errors("Compare instruction expects a register as it's first operand. Unknown operand.");
        return;
    }
    next_token();
    // now the next token can be either an immediate, variable or a register
    std::unique_ptr<nodes::Base> node;
    nodes::NodeKind kind = nodes::_INST_CMP_IMM;
    if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT || curr_tok.type == tokens::_TT_FLOAT || curr_tok.type == tokens::_TT_NFLOAT)
    {
        // we have an immediate
        node = std::make_unique<nodes::NodeCmpImm>();
        auto temp = (nodes::NodeCmpImm *)node.get();
        temp->regr = regr->second;
        temp->val = curr_tok.value;
    }
    else if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        // we either have a variable or a register
        auto regr2 = nodes::_regr_iden_map.find(curr_tok.value);
        if (regr2 == nodes::_regr_iden_map.end())
        {
            // it's a variable
            node = std::make_unique<nodes::NodeCmpImm>();
            auto temp = (nodes::NodeCmpImm *)node.get();
            temp->is_iden = true;
            temp->regr = regr->second;
            temp->val = curr_tok.value;
        }
        else
        {
            node = std::make_unique<nodes::NodeCmpRegr>();
            auto temp = (nodes::NodeCmpRegr *)node.get();
            temp->regr1 = regr->second;
            temp->regr2 = regr2->second;
            kind = nodes::_INST_CMP_REG;
        }
    }
    else
    {
        send_errors("Needed a register, number or a variable here. What is this?");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, kind, std::move(node), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_jX()
{
    // every jmp can use NodeJmp
    auto curr = curr_tok.type;
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        // we expected an identifier as a label
        send_errors("Expected a label after a jX instruction.");
        return;
    }
    // we also need to make sure that the ID is not a register
    if (nodes::_regr_iden_map.find(curr_tok.value) != nodes::_regr_iden_map.end())
    {
        // we don't need registers either
        send_errors("Expected a label as operand, not register");
        return;
    }
    // we have everything needed
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeControlFlow>();
    auto temp = (nodes::NodeControlFlow *)ptr.get();
    temp->_jmp_label_ = curr_tok.value;
    nodes::NodeKind k = (nodes::NodeKind)(curr + 14);
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_incdec(std::string _m, nodes::NodeKind k)
{
    // this only works on registers
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a register for " + _m + " operation.");
        return;
    }
    auto det = nodes::_regr_iden_map.find(curr_tok.value);
    if (det == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register for " + _m + " operation.");
        return;
    }
    std::unique_ptr<nodes::Base> node = std::make_unique<nodes::NodeOneRegrOperands>();
    ((nodes::NodeOneRegrOperands *)node.get())->oper_rger = det->second;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(node)));
}

void zeta::parser::Parser::handle_inst_logical()
{
    auto temp_curr = curr_tok.type;
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    nodes::NodeKind k;
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a register as the destination.");
        return;
    }
    auto dest = nodes::_regr_iden_map.find(curr_tok.value);
    if (dest == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a destination register for this logical operation.");
        return;
    }
    next_token();
    // either a register or an immediate
    if (curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto src = nodes::_regr_iden_map.find(curr_tok.value);
        if (src == nodes::_regr_iden_map.end())
        {
            send_errors("Expected a source register in this logical instruction.");
            return;
        }
        switch (temp_curr)
        {
        case tokens::_TT_INST_AND:
            k = nodes::_INST_AND_REG;
            break;
        case tokens::_TT_INST_OR:
            k = nodes::_INST_OR_REG;
            break;
        case tokens::_TT_INST_XOR:
            k = nodes::_INST_XOR_REG;
            break;
        }
        ptr = std::make_unique<nodes::NodeLogicalRegReg>();
        auto temp = (nodes::NodeLogicalRegReg *)ptr.get();
        temp->dest_regr = dest->second;
        temp->src_reg = src->second;
    }
    else if (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT)
    {
        switch (temp_curr)
        {
        case tokens::_TT_INST_AND:
            k = nodes::_INST_AND_IMM;
            break;
        case tokens::_TT_INST_OR:
            k = nodes::_INST_OR_IMM;
            break;
        case tokens::_TT_INST_XOR:
            k = nodes::_INST_XOR_IMM;
            break;
        }
        ptr = std::make_unique<nodes::NodeLogicalRegImm>();
        auto temp = (nodes::NodeLogicalRegImm *)ptr.get();
        temp->dest_regr = dest->second;
        temp->value = curr_tok.value;
    }
    else
    {
        send_errors("Expected an integer in logical instruction.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_shift(std::string _m, nodes::NodeKind k)
{
    next_token();
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeShifts>();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a register for " + _m + " operation.");
        return;
    }
    auto dest = nodes::_regr_iden_map.find(curr_tok.value);
    if (dest == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register for " + _m + " operation.");
        return;
    }
    next_token();
    if (curr_tok.type == tokens::_TT_INT)
    {
        auto temp = (nodes::NodeShifts *)ptr.get();
        temp->dest_regr = dest->second;
        temp->value = curr_tok.value;
    }
    else
    {
        send_errors("Expected a positive integer for " + _m + " operation.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_sva_svc()
{
    auto temp = curr_tok;
    next_token();
    // These both instructions only take +ve values
    if (curr_tok.type != tokens::_TT_INT)
    {
        send_errors("Expected a positive value here.");
        return;
    }
    std::unique_ptr<nodes::Base> ptr;
    ptr = std::make_unique<nodes::NodeOneImmOperand>();
    ((nodes::NodeOneImmOperand *)ptr.get())->imm = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, (temp.type == tokens::_TT_INST_SVA) ? nodes::_INST_SVA : nodes::_INST_SVC, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_push()
{
    auto temp = curr_tok;
    nodes::NodeKind k;
    if (temp.type != tokens::_TT_INST_PUSHA)
        next_token();
    std::unique_ptr<nodes::Base> ptr;
    if (temp.type == tokens::_TT_INST_PUSH && (curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT))
    {
        k = nodes::_INST_PUSH_IMM; //  This is push immediate
        ptr = std::make_unique<nodes::NodeOneImmOperand>();
        auto temp = (nodes::NodeOneImmOperand *)ptr.get();
        temp->imm = curr_tok.value;
    }
    else if (temp.type == tokens::_TT_INST_PUSH && curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto regr = nodes::_regr_iden_map.find(curr_tok.value);
        if (regr == nodes::_regr_iden_map.end())
        {
            send_errors("Expected either a number or a register.");
            return;
        }
        k = nodes::_INST_PUSH_REG;
        ptr = std::make_unique<nodes::NodeOneRegrOperands>();
        auto temp = (nodes::NodeOneRegrOperands *)ptr.get();
        temp->oper_rger = regr->second;
    }
    else if (temp.type == tokens::_TT_INST_PUSHA)
    {
        k = nodes::_INST_PUSHA;
        ptr = std::make_unique<nodes::Base>();
    }
    else
    {
        send_errors("Invalid push instruction. Expected a register or a number");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr), lexer.get_current_line_no()));
}

void zeta::parser::Parser::handle_inst_pop()
{
    auto temp = curr_tok;
    nodes::NodeKind k;
    if (temp.type != tokens::_TT_INST_POPA)
        next_token();
    std::unique_ptr<nodes::Base> ptr;
    if (temp.type == tokens::_TT_INST_POP && curr_tok.type == tokens::_TT_IDENTIFIER)
    {
        auto regr = nodes::_regr_iden_map.find(curr_tok.value);
        if (regr == nodes::_regr_iden_map.end())
        {
            send_errors("Expected a register as the destination.");
            return;
        }
        k = nodes::_INST_POP;
        ptr = std::make_unique<nodes::NodeOneRegrOperands>();
        auto temp = (nodes::NodeOneRegrOperands *)ptr.get();
        temp->oper_rger = regr->second;
    }
    else if (temp.type == tokens::_TT_INST_POPA)
    {
        k = nodes::_INST_POPA;
        ptr = std::make_unique<nodes::Base>();
    }
    else
    {
        send_errors("Invalid pop instruction. Expected a register.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_lea()
{
    next_token();
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeLea>();
    auto temp = (nodes::NodeLea *)ptr.get();
    nodes::Registers dest, base, index, scale;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as Destination.");
        return;
    }
    dest = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as Base.");
        return;
    }
    base = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as index.");
        return;
    }
    index = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as scale.");
        return;
    }
    scale = regr->second;
    temp->base = base;
    temp->dest = dest;
    temp->index = index;
    temp->scale = scale;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_LEA, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_excg()
{
    next_token();
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeExcg>();
    auto temp = (nodes::NodeExcg *)ptr.get();
    nodes::Registers regr1, regr2;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the operand.");
        return;
    }
    regr1 = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the operand.");
        return;
    }
    regr2 = regr->second;
    temp->regr1 = regr1;
    temp->regr2 = regr2;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_EXCG, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_excgX()
{
    auto x = curr_tok;
    next_token();
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeExcg>();
    auto temp = (nodes::NodeExcg *)ptr.get();
    nodes::Registers regr1, regr2;
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the operand.");
        return;
    }
    regr1 = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here as the operand.");
    }
    nodes::NodeKind k = x.type == tokens::_TT_INST_EXCG16 ? nodes::_INST_EXCG16 : x.type == tokens::_TT_INST_EXCG32 ? nodes::_INST_EXCG32
                                                                                                                    : nodes::_INST_EXCG8;
    regr2 = regr->second;
    temp->regr1 = regr1;
    temp->regr2 = regr2;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, k, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_intr()
{
    // reuse push immediate
    next_token();
    std::unique_ptr<nodes::Base> ptr;
    if ((curr_tok.type == tokens::_TT_INT || curr_tok.type == tokens::_TT_NINT))
    {
        ptr = std::make_unique<nodes::NodeOneImmOperand>();
        auto temp = (nodes::NodeOneImmOperand *)ptr.get();
        temp->imm = curr_tok.value;
    }
    else
    {
        send_errors("Expected an interrupt number.");
        return;
    }
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_INTR, std::move(ptr)));
}

void zeta::parser::Parser::handle_inst_cmpxchg()
{
    // This requires three operands
    // two registers and one variable
    nodes::Registers r1, r2;
    std::unique_ptr<nodes::Base> ptr = std::make_unique<nodes::NodeCmpXchg>();
    auto temp = (nodes::NodeCmpXchg *)ptr.get();
    next_token();
    auto regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here.");
        return;
    }
    r1 = regr->second;
    next_token();
    regr = nodes::_regr_iden_map.find(curr_tok.value);
    if ((regr) == nodes::_regr_iden_map.end())
    {
        send_errors("Expected a register here.");
        return;
    }
    r2 = regr->second;
    next_token();
    if (curr_tok.type != tokens::_TT_IDENTIFIER)
    {
        send_errors("Expected a label here in cmpxchg instruction.");
        return;
    }
    if (nodes::_regr_iden_map.find(curr_tok.value) != nodes::_regr_iden_map.end())
    {
        send_errors("Expected one label as operand here not three register.");
        return;
    }
    temp->r1 = r1;
    temp->r2 = r2;
    temp->var_name = curr_tok.value;
    nodes.push_back(std::make_unique<nodes::Node>(nodes::_TYPE_INST, nodes::_INST_CMPXCHG, std::move(ptr), lexer.get_current_line_no()));
}
