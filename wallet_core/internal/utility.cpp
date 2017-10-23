/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#include "wallet_core/internal/utility.h"

#include "wallet_core/error.h"

#include "wally_core.h"

#include <string.h>

namespace wallet_core
{

namespace internal
{
char* copy_string(const std::string& str)
{
    return copy_string(str.c_str());
}

char* copy_string(const char* str)
{
    if (!str)
    {
        return nullptr;
    }

    const size_t len = strlen(str);

    wally_operations wally_ops;
    int result = wally_get_operations(&wally_ops);
    if (result != WALLY_OK || !wally_ops.malloc_fn)
    {
        throw std::runtime_error("Failed to copy string.");
    }

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        throw std::runtime_error("Failed to allocate memory.");
    }

    memcpy(new_message, str, len);
    new_message[len] = '\0';
    return new_message;
}

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
        const char* message = copy_string(exception.what());
        if (!message)
        {
            // We have to guess here, most likely out of memory.
            return make_error(ERROR_OUT_OF_MEMORY, "Out of memory");
        }

        Error* error = make_error(ERROR_GENERAL_ERROR, message);
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
