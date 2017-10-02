#ifndef WALLET_KEYS_H
#define WALLET_KEYS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Key;
struct Error;
struct BinaryData;

enum ChildKeyType
{
    CHILD_KEY_TYPE_PRIVATE,
    CHILD_KEY_TYPE_PUBLIC,
};

Error* make_master_key(const BinaryData* seed, Key** key);
Error* make_child_key(const Key* parent_key, ChildKeyType type,
        uint32_t chain_code, Key** key);
Error* key_to_string(const Key*, const char **str);

void free_key(Key* root);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_KEYS_H
