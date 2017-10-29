/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_KEYS_H
#define MULTY_CORE_KEYS_H

#include "multy_core/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Key;
struct ExtendedKey;
struct PrivateKey;
struct PublicKey;

struct Error;
struct BinaryData;

MULTY_CORE_API Error* make_master_key(
        const BinaryData* seed, ExtendedKey** new_master_key);

MULTY_CORE_API Error* make_child_key(
        const ExtendedKey* parent_key,
        uint32_t chain_code,
        ExtendedKey** new_child_key);

MULTY_CORE_API Error* extended_key_to_string(
        const ExtendedKey* extended_key, const char** new_str);

// Not sure if we need this, since private key type depends on the account type.
// MULTY_CORE_API Error* extended_to_private_key(
//        const ExtendedKey* key, PrivateKey** new_key);

MULTY_CORE_API Error* private_to_public_key(
        const PrivateKey* private_key, PublicKey** new_public_key);

/** Here and below, a Key* is either PrivateKey* or PublicKey*
 *  but NOT ExtendedKey*.
*/
MULTY_CORE_API Error* key_to_string(const Key* key, const char** new_str);

//MULTY_CORE_API Error* sign_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_signature);

//MULTY_CORE_API Error* encrypt_with_key(
//        const Key* key,
//        const BinaryData* data,
//        BinaryData** new_encrypted_data);

//MULTY_CORE_API Error* decrypt_with_key(
//        const Key* key,
//        const BinaryData* data,
//        BinaryData** new_decrypted_data);

MULTY_CORE_API void free_extended_key(ExtendedKey* root);
MULTY_CORE_API void free_key(Key* root);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_KEYS_H
