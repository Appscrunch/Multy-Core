#ifndef WALLET_KEYS_H
#define WALLET_KEYS_H

#include "error.h"
#include "common.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Key;

//struct KeyNode {
//    const Key key;
//    const KeyNode* child_nodes;
//    const size_t chidren_count;
//};

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
//void free_node_tree(KeyNode* root);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_KEYS_H
