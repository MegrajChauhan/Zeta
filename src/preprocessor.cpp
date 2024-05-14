#include "../includes/preprocessor.hpp"

zeta::error::_STATES_ zeta::prep::Preprocessor::prepare(std::string file)
{
    std::filesystem::path _p = std::filesystem::current_path() / file;
    file = _p.generic_string();
    if (error::is_file_already_included(file))
        return error::_TRUE_; // don't process already included file
    // make valid checks
    if (!file.ends_with(".zasm"))
        return error::_FILE_INVALID_;
    if (!std::filesystem::exists(_p))
        return error::_FILE_NOT_AVAI_;
    if (std::filesystem::is_directory(_p))
        return error::_FILE_A_DIR_;
    error::append_file(file); // register the new file after all validity checks
    std::fstream _in(file, std::ios::in);
    std::string _t;
    while (!_in.eof())
    {
        std::getline(_in, _t);
        file_contents.push_back(_t);
    }
    _in.close();
    // we have the file's contents
    return error::_NORMAL_;
}

zeta::error::_STATES_ zeta::prep::Preprocessor::process()
{
    auto it = file_contents.begin();
    error::_STATES_ err = error::_NORMAL_;
    while (it != file_contents.end())
    {
        col = 0;
        auto _st = it->begin();
        std::string word;
        while (*_st == ' ' || *_st == '\t')
        {
            _st++;
            col++;
        }
        if (*_st == ';' && _st != it->end() && *(_st + 1) == ';')
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
                while (_st != it->end())
                {
                    path += *_st;
                    _st++;
                    col++;
                }
                if (path.size() == 0)
                {
                    error::add_error(error::Error(error::_EXPECTED_PATH_GOT_NL_, "Expected a path to include.", *it, line, col));
                    err = error::_ERR_;
                }
                else
                {
                    Preprocessor _p;
                    auto res = _p.prepare(path);
                    if (res != error::_NORMAL_ && res != error::_TRUE_)
                    {
                        error::add_error(error::Error(res, " ", *it, line, col - path.length()));
                        err = error::_ERR_;
                    }
                    else if (res != error::_TRUE_)
                    {
                        _p.refr_line = line + refr_line + 1;
                        if ((res = _p.process()) != error::_NORMAL_)
                            err = error::_ERR_;
                        else
                        {
                            *it = '\n';
                            file_contents.insert(it + 1, _p.file_contents.begin(), _p.file_contents.end());
                            it = file_contents.begin() + line + _p.file_contents.size();
                            _FDetails_ det;
                            det.end = refr_line + line + _p.line;
                            det.st = refr_line + line + 1;
                            fdetails[(std::filesystem::current_path() / path).generic_string()] = det;
                            fdetails.merge(_p.fdetails);
                            line += _p.line;
                        }
                        error::_update_inc_tree_(); // update before removing
                        error::remove_file();
                    }
                    else
                    {
                        *it = "\n";
                    }
                }
            }
        }
        it++;
        line++;
    }
    return err;
}
/*
    while (true)
    {
        auto _l = file_contents[line];
        if (_l == "!!~~")
            break;
        // check if there is "use" here
    file_contents.pop_back();
*/