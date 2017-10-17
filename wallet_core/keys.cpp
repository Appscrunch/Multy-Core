#include "keys.h"

#include "common.h"
#include "error.h"

#include "internal/key.h"
#include "internal/utility.h"

#include "wally_core.h"
#include "wally_bip32.h"

#include <memory>

static_assert(KEY_TYPE_PRIVATE == BIP32_FLAG_KEY_PRIVATE,
        "invalid KEY_TYPE_PRIVATE value");
static_assert(KEY_TYPE_PUBLIC == BIP32_FLAG_KEY_PUBLIC,
        "invalid KEY_TYPE_PUBLIC value");

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_master_key(const BinaryData* seed, Key** master_key)
{
    ARG_CHECK(seed);
    ARG_CHECK(master_key);

    try
    {
        auto key = null_unique_ptr<Key>(free_key);
        key.reset(new Key);
        const int result = bip32_key_from_seed(seed->data, seed->len,
                BIP32_VER_MAIN_PRIVATE, 0, &key->key);
        if (result == WALLY_ERROR)
        {
            return make_error(ERROR_BAD_ENTROPY,
                    "Can't generate master key with given entropy");
        }
        throw_if_wally_error(result, "Failed to generate master key");
        *master_key = key.release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

Error* make_child_key(const Key* parent_key, KeyType type,
        uint32_t chain_code, Key** child_key)
{
    try
    {
        ARG_CHECK(parent_key);
        ARG_CHECK(child_key);

        auto key = null_unique_ptr<Key>(free_key);
        key.reset(new Key);
        throw_if_wally_error(
                bip32_key_from_parent(&parent_key->key, chain_code, type,
                        &key->key),
                "Failed to make child key");

        *child_key = key.release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

Error* key_to_base58(const Key* key, KeyType type, const char **str)
{
    static const size_t size = BIP32_SERIALIZED_LEN;
    unsigned char serialized_key[size];

    ARG_CHECK(key);
    ARG_CHECK(str);

    try
    {
        const uint32_t flag = (type == KEY_TYPE_PRIVATE) ?
                BIP32_FLAG_KEY_PRIVATE : BIP32_FLAG_KEY_PUBLIC;
        throw_if_wally_error(
                bip32_key_serialize(&key->key, flag, serialized_key, size),
                "Failed to serialize key");

        throw_if_wally_error(
                wally_base58_from_bytes(serialized_key, size, 0,
                        const_cast<char**>(str)),
                "Failed to covert key to string");
    }
    catch (...)
    {
        return exception_to_error();
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
