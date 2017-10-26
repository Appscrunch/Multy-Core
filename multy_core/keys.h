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
struct Error;
struct BinaryData;

enum KeyType
{
    KEY_TYPE_PRIVATE,
    KEY_TYPE_PUBLIC,
};

MULTY_CORE_API Error* make_master_key(const BinaryData* seed, Key** key);
MULTY_CORE_API Error* make_child_key(
        const Key* parent_key, KeyType type, uint32_t chain_code, Key** key);
MULTY_CORE_API Error* key_to_base58(
        const Key*, KeyType type, const char** str);

MULTY_CORE_API void free_key(Key* root);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_KEYS_H
