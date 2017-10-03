#ifndef WALLET_ERROR_H
#define WALLET_ERROR_H

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
};

Error* make_error(ErrorCode code, const char* message);
void free_error(Error* error);

Error* internal_make_error(int code, const char* message);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WAALLET_ERROR_H
