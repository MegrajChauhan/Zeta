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
    for (auto _l : file_contents)
    {
        if (_l.contains('$'))
        {
            // we should process this line only if it contains '$'
        }
    }
}

/*
GENERAL RULES FOR DIRECTIVES:
If, say a.zasm, includes, say b.zasm, which includes, say c.zasm, and if c.zasm includes a.zasm which will the include b.zasm, this will throw an error
For a preprocessing directive, a line must start with '$' with any number of spaces, tabs before and after.
After the '$' symbol and all the spaces, preprocessing directives are expected
Multiple preprocessing directives cannot be in the same line
*/