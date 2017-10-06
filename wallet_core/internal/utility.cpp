#include "utility.h"

#include "error.h"

#include "wally_core.h"

#include <string.h>

namespace wallet_core
{

namespace internal
{

Error* exception_to_error()
{
    try
    {
        throw;
    }
    catch (Error* error)
    {
        return error;
    }
    catch (std::exception const& exception)
    {
        // Copy a message from exception to a new buffer.
        // Buffer memory has to be allocated by libwally, since we want
        // to use free_string() consistently on all strings,
        // even if those were allocated internaly.
        const char* message = exception.what();
        const size_t len = strlen(message);

        wally_operations wally_ops;
        int result = wally_get_operations(&wally_ops);
        if (result != WALLY_OK || !wally_ops.malloc_fn)
        {
            return internal_make_error(result, "Unable to handle exception.");
        }

        char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
        if (!new_message)
        {
            return make_error(ERROR_OUT_OF_MEMORY,
                    "Not enough memory to process exception properly.");
        }
        memcpy(new_message, message, len);

        Error* error = make_error(ERROR_GENERAL_ERROR, new_message);
        error->owns_message = true;
        return error;
    }
    catch (...)
    {
        return make_error(ERROR_GENERAL_ERROR, "Unknown exception");
    }
}

} // namespace internal

} // namespace wallet_core
