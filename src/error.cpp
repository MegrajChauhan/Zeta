#include "../includes/error.hpp"

INTERNAL_FUNC void _err_file_invalid_(zeta::error::Error _e)
{
    /*
      file:line:col: Error: <Extra message> message
      line
      ^~~~ [Maybe extra info here]
    */
    std::cerr << _CCODE_BOLDWHITE << _e.error_file << ":" << _e.line + 1 << ":" << _e.col << ":" << _CCODE_RESET << _CCODE_BOLDRED << "ERROR" << _CCODE_RESET << ": " << _CCODE_BOLDCYAN << _e.extra_error_msg << _CCODE_RESET;
    std::cerr << "\nThe file being included is not a valid file." << "\n\n";
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

INTERNAL_FUNC void _err_file_not_avai_(zeta::error::Error _e)
{
    std::cerr << _CCODE_BOLDWHITE << _e.error_file << ":" << _e.line + 1 << ":" << _e.col << ":" << _CCODE_RESET << _CCODE_BOLDRED << "ERROR" << _CCODE_RESET << ": " << _CCODE_BOLDCYAN << _e.extra_error_msg << _CCODE_RESET;
    std::cerr << "\nThe file being included doesn't exist." << "\n\n";
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

INTERNAL_FUNC void _err_file_a_dir_(zeta::error::Error _e)
{
    std::cerr << _CCODE_BOLDWHITE << _e.error_file << ":" << _e.line + 1 << ":" << _e.col << ":" << _CCODE_RESET << _CCODE_BOLDRED << "ERROR" << _CCODE_RESET << ": " << _CCODE_BOLDCYAN << _e.extra_error_msg << _CCODE_RESET;
    std::cerr << "\nThe file being included is a directory." << "\n\n";
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

INTERNAL_FUNC void _err_expected_path_got_nl_(zeta::error::Error _e)
{
    std::cerr << _CCODE_BOLDWHITE << _e.error_file << ":" << _e.line + 1 << ":" << _e.col << ":" << _CCODE_RESET << _CCODE_BOLDRED << "ERROR" << _CCODE_RESET << ": " << _CCODE_BOLDCYAN << _e.extra_error_msg << _CCODE_RESET;
    std::cerr << "\n"
              << _e.error_msg << "\n\n";
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
            _err_file_invalid_(_e);
            break;
        case _FILE_NOT_AVAI_:
            any_fatal = true;
            _err_file_not_avai_(_e);
            break;
        case _FILE_A_DIR_:
            any_fatal = true;
            _err_file_a_dir_(_e);
            break;
        case _EXPECTED_PATH_GOT_NL_:
            any_fatal = true;
            _err_expected_path_got_nl_(_e);
            break;
        }
    }
    if (any_fatal)
        exit(EXIT_FAILURE);
    err.clear();
    _inc_files_.clear();
    return;
}