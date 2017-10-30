/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/keys.h"

#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

#include "wally_bip32.h"
#include "wally_core.h"

#include <memory>

// static_assert(
//        KEY_TYPE_PRIVATE == BIP32_FLAG_KEY_PRIVATE,
//        "invalid KEY_TYPE_PRIVATE value");
// static_assert(
//        KEY_TYPE_PUBLIC == BIP32_FLAG_KEY_PUBLIC,
//        "invalid KEY_TYPE_PUBLIC value");

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_master_key(const BinaryData* seed, ExtendedKey** new_master_key)
{
    ARG_CHECK(seed);
    ARG_CHECK(new_master_key);

    try
    {
        ExtendedKeyPtr key(new ExtendedKey);
        const int result = bip32_key_from_seed(
                seed->data, seed->len, BIP32_VER_MAIN_PRIVATE, 0, &key->key);
        if (result == WALLY_ERROR)
        {
            return make_error(
                    ERROR_BAD_ENTROPY,
                    "Can't generate master key with given entropy");
        }
        throw_if_wally_error(result, "Failed to generate master key");
        *new_master_key = key.release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_master_key);

    return nullptr;
}

Error* make_child_key(
        const ExtendedKey* parent_key,
        uint32_t chain_code,
        ExtendedKey** new_child_key)
{
    ARG_CHECK(parent_key);
    ARG_CHECK(new_child_key);

    try
    {
        ExtendedKeyPtr key(new ExtendedKey);
        throw_if_wally_error(
                bip32_key_from_parent(
                        &parent_key->key, chain_code, BIP32_FLAG_KEY_PRIVATE,
                        &key->key),
                "Failed to make child key");

        *new_child_key = key.release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_child_key);

    return nullptr;
}

Error* extended_key_to_string(
        const ExtendedKey* extended_key, const char** new_str)
{
    ARG_CHECK(extended_key);
    ARG_CHECK(new_str);

    try
    {
        *new_str = copy_string(extended_key->to_string());
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_str);

    return nullptr;
}

Error* private_to_public_key(
        const PrivateKey* private_key, PublicKey** new_public_key)
{
    ARG_CHECK(private_key);
    ARG_CHECK(new_public_key);
    try
    {
        *new_public_key = private_key->make_public_key().release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_public_key);

    return nullptr;
}

Error* key_to_string(const Key* key, const char** new_str)
{
    ARG_CHECK(key);
    ARG_CHECK(new_str);
    try
    {
        *new_str = copy_string(key->to_string());
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_str);

    return nullptr;
}

//Error* sign_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_signature)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_signature);
//    try
//    {
//        *new_signature = key->sign(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

//Error* encrypt_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_encrypted_data)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_encrypted_data);
//    try
//    {
//        *new_encrypted_data = key->encrypt(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

//Error* decrypt_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_decrypted_data)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_decrypted_data);
//    try
//    {
//        *new_decrypted_data = key->decrypt(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

void free_extended_key(ExtendedKey* key)
{
    delete key;
}

void free_key(Key* key)
{
    delete key;
}
