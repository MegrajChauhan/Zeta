#include "../includes/preprocessor.hpp"

zeta::prep::Reader::Reader(std::string _inp_path_)
{
    this->_path_to_file_ = _inp_path_;
}

bool zeta::prep::Reader::is_valid_file()
{
    return _path_to_file_.ends_with(".zasm");
}

bool zeta::prep::Reader::validate_existence()
{
    std::filesystem::path _p = std::filesystem::current_path() / _path_to_file_;
    return std::filesystem::exists(_p);
}

bool zeta::prep::Reader::is_file_a_file()
{
    std::filesystem::path _p = std::filesystem::current_path() / _path_to_file_;
    return !std::filesystem::is_directory(_p);
}

bool zeta::prep::Reader::read()
{
    // The caller should validate for the file's existence
    std::fstream _f(_path_to_file_, std::ios::in);
    // above shouldn't fail
    if (surelyF(!_f.is_open()))
        return false;
    while (!_f.eof())
    {
        // keep reading lines
        std::string line;
        std::getline(_f, line);
        _file_contents_.push_back(line);
    }
    _f.close();
    return true;
}

zeta::prep::Preprocessor::Preprocessor(std::string file_path)
{
    this->inp_file = file_path;
}

bool zeta::prep::Preprocessor::process()
{
    Reader _r(inp_file);
    if (!_r.is_valid_file())
    {
        state = error::_FILE_INVALID_;
        return false;
    }
    if (!_r.validate_existence())
    {
        state = error::_FILE_NOT_AVAI_;
        return false;
    }
    if (!_r.is_file_a_file())
    {
        state = error::_FILE_A_DIR_;
        return false;
    }
    if (!_r.read())
    {
        state = error::_READ_FAILED_;
        return false;
    }
    file_contents = _r.get_file_contents();
    // we have the file now
    // we will process line by line
    if (file_contents.size() == 0)
        return true;
    file_contents.push_back("!!~~");
    while (true)
    {
        auto _l = file_contents[line];
        if (_l == "!!~~")
            break;
        col = 0;
        auto _st = _l.begin();
        // check if there is "use" here
        std::string word;
        // If the line is including some other file, then use must be the first word
        // There can't be multiple "use" in the same line
        while (*_st == ' ' || *_st == '\t')
        {
            _st++;
            col++;
        }
        if (*_st == ';' && _st != _l.end() && *(_st + 1) == ';')
        {
            // skip everything here
        }
        else if (std::isalpha(*_st))
        {
            while (std::isalpha(*_st))
            {
                word += *_st;
                _st++;
                col++;
            }
            if (word == "use")
            {
                // get the path
                std::string path;
                while (*_st == ' ' || *_st == '\t')
                {
                    _st++;
                    col++;
                }
                // until the end of line
                while (_st != _l.end())
                {
                    path += *_st;
                    _st++;
                    col++;
                }
                Preprocessor _p(path);
                if (std::find(included_files.begin(), included_files.end(), path) == included_files.end())
                {
                    if (!_p.process())
                    {
                        err_col = _p.err_col;
                        err_line = _p.err_line;
                        err_msg = _p.err_msg;
                        wrong_line = _p.wrong_line;
                        state = _p.state;
                        return false;
                    }
                    std::string _ = "#" + path;
                    included_files.push_back(path);
                    _p.file_contents.insert(_p.file_contents.begin(), _); // for the lexer
                    _p.file_contents.insert(_p.file_contents.end(), "--");
                    file_contents.erase(file_contents.begin() + line);
                    file_contents.insert(file_contents.begin() + line, _p.file_contents.begin(), _p.file_contents.end());
                }
                else
                {
                    file_contents[line] = '\n';
                }
            }
        }
        line++;
    }
    file_contents.pop_back();
    return true;
}

/*
GENERAL RULES FOR DIRECTIVES:
If, say a.zasm, includes, say b.zasm, which includes, say c.zasm, and if c.zasm includes a.zasm which will the include b.zasm, this will throw an error
For a preprocessing directive, a line must start with '$' with any number of spaces, tabs before and after.
After the '$' symbol and all the spaces, preprocessing directives are expected
Multiple preprocessing directives cannot be in the same line
*/