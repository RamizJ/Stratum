#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <error.h>
#include <stdata_global.h>

namespace StData {

class STDATA_GLOBAL_H TerminateException : std::exception
{
    TerminateException();
};

}

#endif // EXCEPTIONS_H
