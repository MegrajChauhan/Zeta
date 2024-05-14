#include "../includes/lexer.hpp"

/*
  The approach that I have taken here has it's own flaws.
  For those interested, there are literally hundreds of ways to solve this(Preprocessing) problem.
  The one used here is pretty intuitive and easy to follow.
  However, this isn't really the method that you would wanna use when creating a production ready assembler.
  Of course it is simple but it isn't really efficient and fast.
*/

/// NOTE: For error handling of errors generated in Parsers, Semantic analysers and Codegen
// The error is propagated to the lexer where the error will be passed to the error handler.
// That makes it such that the other components can remain oblivious about the file contents
// and the task of producing meaningful errors.

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
                col -= val.length();
                _add_error_(error::_INVALID_TOKEN_, "There are more than 1 floating points. Invalid number");
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
            if (it == iter->end())
            {
                col -= string.length();
                _add_error_(error::_STRING_NOT_TERMINATED_, "The string was never terminated.");
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
        // The error handling in this case is very tough
        // And so we will just avoid highlighting
        _add_error_(error::_STRING_NOT_TERMINATED_, "The string was never terminated.");
    }
    return string;
}

zeta::lexer::Token zeta::lexer::Lexer::next_token()
{
    Token token;
    std::string val;
    if (iter != file_contents.end() && should_skip((*it)))
    {
        clear_unnecessary();
    }
    if (iter != file_contents.end() && *it == ';' && peek() == ';')
    {
        // this is a comment and we can completely skip this
        while (iter != file_contents.end() && *it == ';' && peek() == ';')
        {
            consume_comments();
            clear_unnecessary();
        }
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
        _add_error_(error::_INVALID_TOKEN_, "Cannot construct a token from this.");
        consume();
        token.type = tokens::_TT_ERR;
    }
    return token;
}

std::pair<std::string, zeta::prep::_FDetails_> zeta::lexer::Lexer::get_file()
{
    // go through the dets and return the file that is closest to the current line
    std::pair<std::string, prep::_FDetails_> _ret;
    _ret.first = _base_file_name_;
    long diff = line;
    for (auto _d : dets)
    {
        if (_d.second.st <= line && line <= _d.second.end)
        {
            long d = line - _d.second.end;
            if (d < diff && d >= 0)
            {
                diff = d;
                _ret = _d;
            }
        }
    }
    /// NOTE: Here is the problem: Say the error occured in the main file rather than an included file
    // What to do if we wish to get the correct line number? We have no choice but to go file by file, add their total lines and subtract with our line_no
    // It is easy WHEN there are only a few files, what happens when there are multiple files involved? It gets slow and inefficient.
    // What about knowing the include tree? While preprocessing it was easier but while lexing, it isn't that simple.
    // But I have a solution which, obviously, doesn't scale with the size of the input.
    // With just a few(say two Dozen) files(included), it would probably takes 10-20 seconds.
    // Now take big projects with hundreds of files included, it would probably take a few minutes which is not what anyone wants.
    // Can we do anything about that? Absolutely!
    // Will I do it? Maybe. It will be worth it only if the assembler is actually used.
    return _ret;
}

size_t zeta::lexer::Lexer::get_base_file_line_no()
{
    // go through every file that ends before current line_no
    size_t reduce_by = 0;
    for (auto f : dets)
    {
        auto res = inc_tree.find(f.first);
        if (res->second == _base_file_name_)
        {
            if (f.second.end < line)
            {
                // reduce this file's size
                reduce_by += (f.second.end - f.second.st + 1);
            }
        }
    }
    return reduce_by;
}

// simplify error handling process
void zeta::lexer::Lexer::_add_error_(error::_STATES_ _s, std::string msg)
{
    auto _det = get_file();
    size_t _real_line_no_ = 0;
    size_t i = 0;
    if (_det.first == _base_file_name_)
    {
        // This is the base file which is the hard thing
        _real_line_no_ = line - get_base_file_line_no();
        // we don't really have to use _inc_tree_ here
    }
    else
    {
        std::string temp = _base_file_name_;
        _base_file_name_ = _det.first;
        _real_line_no_ = (_det.second.end - _det.second.st) - get_base_file_line_no();
        _base_file_name_ = temp;
        // we need to use _inc_tree_ here
        std::stack<std::string> _l;
        auto res = inc_tree.find(_det.first);
        // Since it must reach the base file, the condition is: check until base file is reached
        while (res->second != _base_file_name_)
        {
            _l.push(res->second);
            res = inc_tree.find(res->second);
        }
        // now populate _files_ with _l
        for (i = 0; i < _l.size(); i++)
        {
            error::append_file(_l.top());
            _l.pop();
        }
        error::append_file(_det.first);
        i++;
    }
    // now we are ready for pushing the error
    error::add_error(error::Error(_s, msg, *iter, _real_line_no_, col));
    for (; i > 0; i--)
    {
        error::remove_file();
    }
}
