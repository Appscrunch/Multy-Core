/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/ethereum_account.h"

#include "multy_core/common.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

extern "C" {
// TODO: Fix issue with not exported symbols from libwally-core for iOS builds,
//   and then re-enable using hex-converting functions from CCAN.
//#include "ccan/str/hex/hex.h"
#include "keccak-tiny/keccak-tiny.h"
} // extern "C"

#include "wally_core.h"
#include "wally_crypto.h"

#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;
typedef std::array<unsigned char, 20> EthereumAddressValue;

struct EthereumPublicKey : public PublicKey
{
public:
    typedef std::array<uint8_t, 33> KeyData;

    EthereumPublicKey(const KeyData& key_data) : m_data(key_data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_hex_from_bytes(
                        m_data.data(), m_data.size(), reset_sp(out_str)),
                "Failed to serialize Enthereum public key");
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

    const KeyData& get_data() const
    {
        return m_data;
    }

private:
    const KeyData m_data;
};

typedef UPtr<EthereumPublicKey> EthereumPublicKeyPtr;

struct EthereumPrivateKey : public PrivateKey
{
    typedef std::array<uint8_t, 33> KeyData;

    EthereumPrivateKey(const KeyData& data) : m_data(data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_hex_from_bytes(
                        m_data.data(), m_data.size(), reset_sp(out_str)),
                "Failed to serialize Ethereum private key");
        return std::string(out_str.get());
    }

    PublicKeyPtr make_public_key() const override
    {
        EthereumPublicKey::KeyData data;
        throw_if_wally_error(
                wally_ec_public_key_from_private_key(
                        m_data.data(), m_data.size(), data.data(),
                        data.max_size()),
                "Failed to derive public key from private key");

        return EthereumPublicKeyPtr(new EthereumPublicKey(data));
        //        return PublicKeyPtr(make_ethereum_public_key().release());
    }

    const BinaryData get_content() const override
    {
        return BinaryData{m_data.data(), m_data.size()};
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

EthereumAddressValue make_address(const BinaryData& key_data)
{
    if (key_data.data == nullptr
        || key_data.len != array_size(EthereumPublicKey::KeyData()))
    {
        throw std::runtime_error(
                "Ethereum: Invalid public key data for making an address");
    }
    std::array<unsigned char, SHA256_LEN> address_hash;
    throw_if_wally_error(
            sha3_256(
                    address_hash.data(), address_hash.size(),
                    // skip first prefix byte.
                    key_data.data + 1, key_data.len - 1),
            "Failed to compute sha3 of public key");

    EthereumAddressValue result;
    static_assert(
            address_hash.size() - result.size() == 12,
            "Invalid EthereumAddressValue size");
    // Copy right 20 bytes
    memcpy(result.data(), address_hash.data() + 12, result.size());
    return result;
}

typedef UPtr<EthereumPrivateKey> EthereumPrivateKeyPtr;
typedef UPtr<EthereumPublicKey> EthereumPublicKeyPtr;

class EthereumAccount : public AccountBase
{
public:
    EthereumAccount(
            EthereumPrivateKeyPtr private_key, const HDPath& path = HDPath())
        : AccountBase(CURRENCY_ETHEREUM, *private_key, path),
          m_private_key(std::move(private_key))
    {
    }

    std::string get_address() const override
    {
        EthereumAddressValue m_address(
                make_address(m_private_key->make_public_key()->get_content()));
        CharPtr out;
        wally_hex_from_bytes(m_address.data(), m_address.size(), reset_sp(out));
        return std::string(out.get());
    }

private:
    EthereumPrivateKeyPtr m_private_key;
};

} // namespace

namespace wallet_core
{
namespace internal
{

EthereumHDAccount::EthereumHDAccount(
        const ExtendedKey& bip44_master_key, uint32_t index)
    : HDAccountBase(bip44_master_key, CURRENCY_ETHEREUM, index)
{
}

AccountPtr EthereumHDAccount::make_account(
        const ExtendedKey& parent_key, AddressType type, uint32_t index) const
{
    ExtendedKeyPtr address_key;
    throw_if_error(make_child_key(&parent_key, index, reset_sp(address_key)));

    EthereumPrivateKey::KeyData data;
    static_assert(sizeof(address_key->key.priv_key) == data.max_size(), "");
    memcpy(data.data(), address_key->key.priv_key, data.size());

    EthereumPrivateKeyPtr private_key(new EthereumPrivateKey(data));

    AccountPtr result(
            new EthereumAccount(
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));

    return std::move(result);
}

} // namespace internal
} // namespace wallet_core
