/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#ifndef WALLET_CORE_KEYS_H
#define WALLET_CORE_KEYS_H

#include "wallet_core/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Key;
struct Error;
struct BinaryData;

enum KeyType
{
    KEY_TYPE_PRIVATE,
    KEY_TYPE_PUBLIC,
};

WALLET_CORE_API Error* make_master_key(const BinaryData* seed, Key** key);
WALLET_CORE_API Error* make_child_key(
        const Key* parent_key, KeyType type, uint32_t chain_code, Key** key);
WALLET_CORE_API Error* key_to_base58(
        const Key*, KeyType type, const char** str);

WALLET_CORE_API void free_key(Key* root);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_CORE_KEYS_H
