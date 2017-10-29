/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/bitcoin_account.h"

#include "multy_core/common.h"

#include "multy_core/internal/hd_path.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

#include "wally_crypto.h"

#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;

struct BitcoinPublicKey : public PublicKey
{
public:
    typedef std::array<uint8_t, 33> KeyData;

    BitcoinPublicKey(const KeyData& key_data) : m_data(key_data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_base58_from_bytes(
                        m_data.data(), m_data.size(), 0, reset_sp(out_str)),
                "Failed to serialize Bitcoin public key");
        return std::string(out_str.get());
    }

    const BinaryData get_content() const override
    {
        return BinaryData{m_data.data(), m_data.size()};
    }

    PublicKeyPtr clone() const override
    {
        return make_clone(*this);
    }

private:
    const KeyData m_data;
};

struct BitcoinPrivateKey : public PrivateKey
{
    typedef std::array<uint8_t, 33> KeyData;

    BitcoinPrivateKey(const KeyData& data) : m_data(data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_base58_from_bytes(
                        m_data.data(), m_data.size(), 0, reset_sp(out_str)),
                "Failed to serialize Bitcoin private key");
        return std::string(out_str.get());
    }

    const BinaryData get_content() const override
    {
        return BinaryData{m_data.data(), m_data.size()};
    }

    PublicKeyPtr make_public_key() const override
    {
        BitcoinPublicKey::KeyData data;
        throw_if_wally_error(
                wally_ec_public_key_from_private_key(
                        m_data.data(), m_data.size(), data.data(),
                        data.max_size()),
                "Failed to derive public key from private key");

        return PublicKeyPtr(new BitcoinPublicKey(data));
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    const KeyData& get_data() const
    {
        return m_data;
    }

private:
    const KeyData m_data;
};

typedef UPtr<BitcoinPrivateKey> BitcoinPrivateKeyPtr;
typedef UPtr<BitcoinPublicKey> BitcoinPublicKeyPtr;

struct BitcoinAccount : public AccountBase
{

    BitcoinAccount(BitcoinPrivateKeyPtr key, HDPath path)
        : AccountBase(CURRENCY_BITCOIN, *key, path),
          m_private_key(std::move(key))
    {
    }

    std::string get_address() const override
    {
        // P2PKH address generated from public key.
        // https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

        unsigned char pub_hash[SHA256_LEN] = {'\0'};
        unsigned char hash_hash[HASH160_LEN] = {'\0'};
        const auto& key_data = m_private_key->get_data();
        throw_if_wally_error(
                // skip the first byte of the pub_key since it contains prefix.
                wally_sha256(
                        key_data.data() + 1, key_data.size() - 1, pub_hash,
                        sizeof(pub_hash)),
                "Hashing public key failed");
        throw_if_wally_error(
                wally_hash160(
                        pub_hash, sizeof(pub_hash), hash_hash,
                        sizeof(hash_hash)),
                "Hashing hash of public key failed");

        auto base58_string_ptr = null_unique_ptr<char>(wally_free_string);
        throw_if_wally_error(
                wally_base58_from_bytes(
                        hash_hash, sizeof(hash_hash), BASE58_FLAG_CHECKSUM,
                        reset_sp(base58_string_ptr)),
                "Converting to base58 failed");
        std::string result(base58_string_ptr.get());
        result.insert(0, 1, '1');

        return result;
    }

private:
    BitcoinPrivateKeyPtr m_private_key;
};

} // namespace

namespace wallet_core
{
namespace internal
{

BitcoinHDAccount::BitcoinHDAccount(
        const ExtendedKey& bip44_master_key, uint32_t index)
    : HDAccountBase(bip44_master_key, CURRENCY_BITCOIN, index)
{
}

BitcoinHDAccount::~BitcoinHDAccount()
{
}

AccountPtr BitcoinHDAccount::make_account(
        const ExtendedKey& parent_key, AddressType type, uint32_t index) const
{
    ExtendedKeyPtr address_key;
    throw_if_error(make_child_key(&parent_key, index, reset_sp(address_key)));

    BitcoinPrivateKey::KeyData data;
    static_assert(sizeof(address_key->key.priv_key) == data.max_size(), "");
    memcpy(data.data(), address_key->key.priv_key, data.size());

    UPtr<BitcoinPrivateKey> private_key(new BitcoinPrivateKey(data));
    return AccountPtr(
            new BitcoinAccount(
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));
}

} // namespace wallet_core
} // namespace internal
