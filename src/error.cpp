#include "../includes/error.hpp"

INTERNAL_FUNC void _err_(zeta::error::Error _e, std::string msg, std::string _lvl)
{
    std::cerr << _CCODE_BOLDWHITE << _e.error_file << ":" << _e.line + 1 << ":" << _e.col << ":" << _CCODE_RESET;
    std::cerr << ((_lvl == "ERROR") ? _CCODE_BOLDRED : (_lvl == "NOTE") ? _CCODE_BOLDCYAN
                                                                        : _CCODE_BOLDGREEN)
              << _lvl << _CCODE_RESET << ": " << _CCODE_BOLDCYAN << _e.extra_error_msg << _CCODE_RESET;
    std::cerr << "\n"
              << msg << "\n\n";
    std::cerr << "\t" << _e.error_line << "\n";
    std::cerr << "\t";
    for (size_t i = 0; i < _e.col; i++)
    {
        putchar(' ');
    }
    std::cout << _CCODE_RED << "^";
    for (size_t i = _e.col + 1; i < _e.error_line.length(); i++)
    {
        putchar('~');
    }
    std::cout << '\n';
}

void zeta::error::add_error(Error _e)
{
    auto l = _files_.size();
    if (l > 1)
    {
        _e.extra_error_msg = "\nIncluded\n";
        for (size_t i = l - 2; i > 0; i--)
        {
            _e.extra_error_msg += "\tfrom " + _files_[i] + '\n';
        }
        _e.extra_error_msg += "\tfrom " + _files_[0];
    }
    else
    {
        _e.extra_error_msg = " ";
    }
    _e.error_file = _files_[l - 1];
    err.push_back(_e);
}

void zeta::error::append_file(std::string file)
{
    _inc_files_.push_back(file);
    _files_.push_back(file);
}

void zeta::error::remove_file()
{
    _files_.pop_back();
}

bool zeta::error::is_file_already_included(std::string file)
{
    return (std::find(_inc_files_.begin(), _inc_files_.end(), file) != _inc_files_.end());
}

void zeta::error::_handle_all_errors_()
{
    bool any_fatal = false;
    for (auto _e : err)
    {
        switch (_e.state)
        {
        case _NORMAL_:
            break;
        case _FILE_INVALID_:
            any_fatal = true;
            _err_(_e, "The file being included is not a valid file.", ERROR);
            break;
        case _FILE_NOT_AVAI_:
            any_fatal = true;
            _err_(_e, "The file being included doesn't exist.", ERROR);
            break;
        case _FILE_A_DIR_:
            any_fatal = true;
            _err_(_e, "The file being included is a directory.", ERROR);
            break;
        case _EXPECTED_PATH_GOT_NL_:
            any_fatal = true;
            _err_(_e, _e.error_msg, ERROR);
            break;
        case _INVALID_TOKEN_:
            any_fatal = true;
            _err_(_e, _e.error_msg, ERROR);
            break;
        case _STRING_NOT_TERMINATED_:
            any_fatal = true;
            _err_(_e, _e.error_msg, ERROR);
            break;
        case _PARSE_ERROR_:
            any_fatal = true; // every parsing error is fatal
            _err_(_e, _e.error_msg, ERROR);
            break;
        }
    }
    if (any_fatal)
        exit(EXIT_FAILURE);
    err.clear();
    return;
}

std::string zeta::error::_get_base_file_() { return _files_[0]; }

void zeta::error::_update_inc_tree_()
{
    // we will make use of _files_ here to build the tree
    // As each new file is included, it is pushed onto _files_.
    // And the file preceding the _files_[_files_.size() - 1] is the file that included the file being pushed
    // Hence, we can use that to build the tree
    // _inc_tree_[_inc_file] = _inc_by;
    _inc_tree_[_files_[_files_.size() - 1]] = _files_[_files_.size() - 2];
}

std::unordered_map<std::string, std::string> zeta::error::_get_inc_tree_() { return _inc_tree_; }