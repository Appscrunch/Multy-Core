/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef WALLET_CORE_ERROR_H
#define WALLET_CORE_ERROR_H

#include "wallet_core/api.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ErrorCode
{
    ERROR_INTERNAL,
    ERROR_INVALID_ARGUMENT,
    ERROR_OUT_OF_MEMORY,
    ERROR_GENERAL_ERROR,
    ERROR_BAD_ENTROPY,
};

/** Error
 * Holds information about error occured inside library.
 */
struct Error
{
    ErrorCode code;
    const char* message;
    bool owns_message;
};

WALLET_CORE_API Error* make_error(ErrorCode code, const char* message);
WALLET_CORE_API void free_error(Error* error);

WALLET_CORE_API Error* internal_make_error(int code, const char* message);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_CORE_ERROR_H
