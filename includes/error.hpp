#ifndef _ERROR_
#define _ERROR_

namespace zeta
{
    namespace error
    {
        enum _STATES_
        {
            _NORMAL_,

            // for preprocessor
            _READ_FAILED_,
            _FILE_INVALID_,
            _FILE_NOT_AVAI_,
            _FILE_A_DIR_,
            _PREPROCESSING_ERR_, // contains program error

        };
    };
}

#endif