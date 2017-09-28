#ifndef WALLET_SEED_GENERATOR_H
#define WALLET_SEED_GENERATOR_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t seed_t;

/** Exposing bare C interface to the users.
 * That greatly simplifies integration with any language/paltform.
 */
#ifdef __cplusplus
extern "C" {
#endif

/** GeneratorError
 * Holds information about error occured inside generator.
 */
struct Error {
	const char* message;
};

/** Binary data, just a pointer and a size in bytes. */
struct BinaryData {
	const void* data;
	size_t len;
};

/** Entropy generator interface.
 * Fill `dest` with `size` random bytes.
 * Caller ensures that `dest` has enough space.
 * Implementation should return 0 on error, or size of generated entropy.
 */
typedef size_t (*EntropySource)(size_t size, void* dest);

/** Generator interface.
 * Each function returns a pointer to a GeneratorError, which is null on no error.
 * User must free returned value with `free_error`.
 *
 * Incoming pointers are not freed under no circumstances, with exception of
 * free_X(X*) functions, which do de-allocate given objects.
 * Please do not use those functions to free any object not created
 * by the Generator.
 */

Error* init();
Error* deinit();

/** Generate a mnemonic for given amount of entropy using default dictionary (english).
 *
 * @param entropySource - bytes of entropy.
 * @param [out]mnemonic - resulting mnemonic, a null-terminated, space-separated
 *	list of words from dictionary.
 * @return - non-null on error.
 */
Error* make_mnemonic(EntropySource entropySource, const char *const* mnemonic);

/** Generates a pseudo-random seed from given mnemonic and password.
 * @param mnemonic - space-separated list of mnemonic words.
 * @param password - password, optional, can be null if not set.
 * @param [out]seed - resulting pseudo-random seed.
 * @return - non-null on error
 */
Error* make_seed(const char* mnemonic, const char* password, BinaryData** seed);

/** Frees GeneratorError, can take null */
void free_error(Error* error);

/** Frees mnemonic, can take null */
void free_mnemonic(const char* mnemonic);

/** Frees BinaryData, can take null */
void free_binarydata(BinaryData*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_SEED_GENERATOR_H
