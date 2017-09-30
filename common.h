#ifndef WALLET_COMMON_H
#define WALLET_COMMON_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Entropy generator interface.
 * Fill `dest` with `size` random bytes.
 * Caller ensures that `dest` has enough space.
 * Implementation should return 0 on error, or size of generated entropy.
 */
typedef size_t (*EntropySource)(size_t size, void* dest);

/** GeneratorError
 * Holds information about error occured inside generator.
 */
struct Error {
    const char* message;
};

/** Binary data, just a pointer and a size in bytes. */
struct BinaryData {
    const unsigned char* data;
    size_t len;
};

/** Frees GeneratorError, can take null */
void free_error(Error* error);

/** Frees BinaryData, can take null */
void free_binarydata(BinaryData*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_COMMON_H
