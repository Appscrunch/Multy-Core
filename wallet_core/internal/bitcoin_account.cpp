#include "bitcoin_account.h"

#include "internal/key.h"
#include "internal/utility.h"

#include "wally_crypto.h"

#include <string>

namespace
{
using namespace wallet_core::internal;
struct BitcoinAddress : public AccountAddress
{
    // using parent constructor
    using AccountAddress::AccountAddress;

    std::string get_address_string() const override
    {
        // P2PKH address generated from public key.
        // https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

        unsigned char pub_hash[SHA256_LEN] = {'\0'};
        unsigned char hash_hash[HASH160_LEN] = {'\0'};
        // skip the first byte of the pub_key since it contains prefix.
        throw_if_wally_error(
                wally_sha256(&extended_key->key.pub_key[1],
                        sizeof(extended_key->key.pub_key) - 1,
                        pub_hash, sizeof(pub_hash)),
                "Hashing public key failed");
        throw_if_wally_error(
                wally_hash160(pub_hash, sizeof(pub_hash),
                        hash_hash, sizeof(hash_hash)),
                "Hashing hash of public key failed");

        auto base58_string_ptr = null_unique_ptr<char>(wally_free_string);
        throw_if_wally_error(
                wally_base58_from_bytes(hash_hash, sizeof(hash_hash),
                        BASE58_FLAG_CHECKSUM, reset_sp(base58_string_ptr)),
                "Converting to base58 failed");
        std::string result(base58_string_ptr.get());
        result.insert(0, 1, '1');

        return result;
    }
    std::string get_path() const override
    {
        std::string result;
        result.reserve(100);
        result.append("m");
        for (auto p : path)
        {
            result.append("/");
            result.append(std::to_string(p));
        }
        return result;
    }
};

} // namespace

namespace wallet_core
{
namespace internal
{

BitcoinAccount::BitcoinAccount(const Key& bip44_master_key, uint32_t index)
    : Account(bip44_master_key, CURRENCY_BITCOIN, index)
{}

BitcoinAccount::~BitcoinAccount()
{}

AccountAddressPtr BitcoinAccount::make_address(const Key& parent_key, uint32_t index)
{
    KeyPtr address_key;

    throw_if_error(make_child_key(&parent_key,
                                  KEY_TYPE_PRIVATE,
                                  index,
                                  reset_sp(address_key)));

    return std::unique_ptr<AccountAddress>(new BitcoinAddress(
            get_path(),
            std::move(address_key)));
}

} // namespace wallet_core
} // namespace internal
