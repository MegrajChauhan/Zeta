#include "../includes/lexer.hpp"

void zeta::lexer::Lexer::consume()
{
    if (it == iter->end())
    {
        // next line
        if (iter != file_contents.end())
        {
            iter++;
            it = iter->begin();
            col = 0;
            line++;
        }
        return;
    }
    it++;
    col++;
}

void zeta::lexer::Lexer::consume_comments()
{
    line++;
    iter++;
    it = iter->begin();
    col = 0;
}

void zeta::lexer::Lexer::clear_unnecessary()
{
    while (iter != file_contents.end() && should_skip(*it))
    {
        consume();
    }
}

char zeta::lexer::Lexer::peek()
{
    if (it == iter->end() || (it + 1) == iter->end())
    {
        consume();
        return *it;
    }
    return *(it + 1);
}

zeta::lexer::Token zeta::lexer::Lexer::get_iden_or_keyword()
{
    Token token;
    std::string iden_or_keyword;
    while (((*it != ' ' && (is_alpha(*it) || is_num(*it) && !is_oper(*it))) || *it == '_'))
    {
        iden_or_keyword.push_back(*it);
        consume();
    }
    auto kind = _iden_map_.find(iden_or_keyword);
    if (kind == _iden_map_.end())
    {
        // since the characters we read are valid then it must be an identifier
        token.type = tokens::_TT_IDENTIFIER;
        token.value = iden_or_keyword;
    }
    else
    {
        // then it is a keyword
        token.type = (*kind).second;
    }
    return token;
}

zeta::lexer::Token zeta::lexer::Lexer::get_number()
{
    Token token;
    size_t float_count = 0;
    std::string val;
    while (is_num(*it) || *it == '.')
    {
        if (*it == '.')
        {
            float_count++;
            if (float_count > 1)
            {
                // invalid_token(val); // exit
            }
        }
        val.push_back(*it);
        consume();
    }
    return Token(float_count == 1 ? tokens::_TT_FLOAT : tokens::_TT_INT, val);
}

std::string zeta::lexer::Lexer::get_current_line()
{
    return *iter;
}

std::string zeta::lexer::Lexer::get_string()
{
    char starting_quote = *it;
    std::string string;
    consume();
    while (*it != starting_quote && it != iter->end())
    {
        if (*it == '\\')
        {
            consume();
            // lex_error("The string was never terminated.");
            if (it == iter->end())
            {
            }
            switch (*it)
            {
            case 'n':
                string += '\n';
                break;
            case 't':
                string += '\t';
                break;
            case 'r':
                string += '\r';
                break;
            case '0':
                string += '\0';
                break;
            default:
                string += *it;
            }
            consume();
        }
        else
        {
            string += *it;
            consume();
        }
    }
    if (*it == starting_quote)
    {
        // the string was indeed terminated
        // we also need to check if there is another '.'
        consume();
        if (*it == '.')
        {
            consume();
            clear_unnecessary();
            consume_comments();
            string += get_string(); // a recursive function call to get all the needed strings
        }
    }
    else
    {
        // lex_error("The string was never terminated. Reached EOF");
    }
    return string;
}

zeta::lexer::Token zeta::lexer::Lexer::next_token()
{

    Token token;
    std::string val;
    if (should_skip((*it)))
    {
        clear_unnecessary();
    }
    if (*it == ';' && peek() == ';')
    {
        // this is a comment and we can completely skip this
        while (*it == ';' && peek() == ';')
            consume_comments();
    }
    if (iter == file_contents.end())
        return Token(_TT_EOF, "");
    if (is_oper(*it))
    {
        val.push_back(*it);
        consume();
        // we don't have multi-character operators and so we can skip it as well
        auto operkind = _iden_map_.find(val);
        // operkind should be valid
        token.type = (*operkind).second;
    }
    else if (is_alpha(*it) || *it == '.' || *it == '_')
    {
        if (*it == '.')
        {
            consume();
        }
        token = get_iden_or_keyword();
    }
    else if (*it == '-')
    {
        consume();
        token = get_number();
        token.value.insert(token.value.begin(), '-');
        token.type = (TokenType)((size_t)token.type + 1);
    }
    else if (is_num(*it))
    {
        token = get_number();
    }
    else if (*it == '\"' || *it == '\'')
    {
        // get the string
        token.type = _TT_STRING;
        token.value = get_string();
    }
    else
    {
        // invalid_token();
    }
    return token;
}

/*
masm::lexer::Token masm::lexer::Lexer::lex()
{
}

*/
