#ifndef WALLET_MNEMONIC_H
#define WALLET_MNEMONIC_H

#include "common.h"

#include <stdint.h>
#include <stddef.h>

/** Exposing bare C interface to the users.
 * That greatly simplifies integration with any language/paltform.
 */
#ifdef __cplusplus
extern "C" {
#endif

struct Error;
struct BinayData;

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
Error* make_mnemonic(EntropySource entropySource, const char ** mnemonic);

/** Generates a pseudo-random seed from given mnemonic and password.
 * @param mnemonic - space-separated list of mnemonic words.
 * @param password - password, optional, can be null if not set.
 * @param [out]seed - resulting pseudo-random seed.
 * @return - non-null on error
 */
Error* make_seed(const char* mnemonic, const char* password, BinaryData** seed);
Error* seed_to_string(const BinaryData* seed, const char** str);

/** Frees mnemonic, can take null */
void free_mnemonic(const char* mnemonic);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_SEED_GENERATOR_H
