#include "keys.h"

#include "common.h"
#include "error.h"
#include "internal/utility.h"

#include "wally_core.h"
#include "wally_bip32.h"

#include <memory>

static_assert(CHILD_KEY_TYPE_PRIVATE == BIP32_FLAG_KEY_PRIVATE,
        "invalid CHILD_KEY_TYPE_PRIVATE value");
static_assert(CHILD_KEY_TYPE_PUBLIC == BIP32_FLAG_KEY_PUBLIC,
        "invalid CHILD_KEY_TYPE_PUBLIC value");

namespace
{
using namespace wallet_core::internal;
} // namespace

struct Key
{
    ext_key key;
};

Error* make_master_key(const BinaryData* seed, Key** master_key)
{
    ARG_CHECK(seed);
    ARG_CHECK(master_key);

    auto key = null_unique_ptr<Key>(free_key);
    const int result = bip32_key_from_seed(seed->data, seed->len,
            BIP32_VER_MAIN_PRIVATE, 0, &key->key);
    if (result == WALLY_ERROR)
    {
        return make_error(ERROR_BAD_ENTROPY,
                "Can't generate master key with given entropy");
    } else if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to generate master key");
    }
    *master_key = key.release();
    return nullptr;
}

Error* make_child_key(const Key* parent_key, ChildKeyType type,
        uint32_t chain_code, Key** child_key)
{
    ARG_CHECK(parent_key);
    ARG_CHECK(child_key);

    auto key = null_unique_ptr<Key>(free_key);
    const int result = bip32_key_from_parent(&parent_key->key, chain_code, type,
            &key->key);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to make child key");
    }

    *child_key = key.release();
    return nullptr;
}

Error* key_to_string(const Key* key, const char **str)
{
    ARG_CHECK(key);
    ARG_CHECK(str);

    static const size_t size = BIP32_SERIALIZED_LEN;
    unsigned char serialized_key[size];

    /// !CAUTION! fragile, based on libwally internal funtion key_is_private().
    const uint32_t flag = (key->key.priv_key[0] == BIP32_FLAG_KEY_PRIVATE) ?
            BIP32_FLAG_KEY_PRIVATE : BIP32_FLAG_KEY_PUBLIC;
    int result = bip32_key_serialize(&key->key, flag, serialized_key, size);
    if (result != WALLY_OK)
    {
        return internal_make_error(result,
                "Failed to convert key to serialize key");
    }
    result = wally_base58_from_bytes(serialized_key, size, 0,
            const_cast<char**>(str));
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to covert key to string");
    }

    return nullptr;
}

void free_key(Key* key)
{
    if (key)
    {
        return;
    }
    delete key;
}
