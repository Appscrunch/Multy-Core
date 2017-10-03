#include "error.h"

#include "wally_core.h"

#include <stdlib.h>

namespace
{

ErrorCode convert_error_code(int code)
{
    switch (code)
    {
        case WALLY_EINVAL:
            return ERROR_INTERNAL;
        case WALLY_ENOMEM:
            return ERROR_OUT_OF_MEMORY;
        case WALLY_OK:
        case WALLY_ERROR:
        default:
            return ERROR_GENERAL_ERROR;
    }
}

} // namespace

Error* make_error(ErrorCode code, const char* message)
{
    Error* result = new Error;
    result->message = message;
    result->code = code;
    return result;
}

Error* internal_make_error(int code, const char* message)
{
    return make_error(convert_error_code(code), message);
}

void free_error(Error* error)
{
    if (!error)
    {
        return;
    }
    // no need to delete error message, since it is always a static string.
    delete error;
}
