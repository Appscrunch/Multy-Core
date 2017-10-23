#include "wallet_core/internal/ethereum_account.h"

#include "wallet_core/internal/utility.h"
#include "wallet_core/internal/key.h"

extern "C" {
#include "ccan/str/hex/hex.h"
#include "keccak-tiny/keccak-tiny.h"
} // extern "C"

#include "wally_crypto.h"

#include <string.h>

namespace
{
using namespace wallet_core::internal;
typedef std::array<unsigned char, 20> EthereumAddressValue;

EthereumAddressValue make_address(const Key& key)
{
    std::array<unsigned char, SHA256_LEN> address_hash;
    throw_if_wally_error(
            sha3_256(address_hash.data(), address_hash.size(),
                    // skip first prefix byte.
                    &key.key.pub_key[1],
                    sizeof(key.key.pub_key) - 1),
            "Failed to compute sha3 of public key");

    EthereumAddressValue result;
    static_assert(address_hash.size() - result.size() == 12, "Invalid EthereumAddressValue size");
    // Copy right 20 bytes
    memcpy(result.data(), address_hash.data() + 12, result.size());
    return result;
}

class EthereumAccountAddress : public AccountAddress
{
public:
    EthereumAccountAddress(const HDPath& path, KeyPtr extended_key)
        : AccountAddress(path, std::move(extended_key)),
          m_address(make_address(get_key()))
    {}

    std::string get_address_string() const override
    {
        std::string result(hex_str_size(m_address.size()), '\0');
        hex_encode(m_address.data(), m_address.size(),
                const_cast<char*>(result.data()), result.size());
        return result;
    }

private:
    const EthereumAddressValue m_address;
};

} // namespace

namespace wallet_core
{
namespace internal
{

EthereumAccount::EthereumAccount(const Key& bip44_master_key, uint32_t index)
    : Account(bip44_master_key, CURRENCY_ETHEREUM, index)
{}

AccountAddressPtr EthereumAccount::make_address(const Key& parent_key, AddressType type, uint32_t index)
{
    KeyPtr address_key;
    throw_if_error(make_child_key(&parent_key,
            KEY_TYPE_PRIVATE,
            index,
            reset_sp(address_key)));

    AccountAddressPtr result(new EthereumAccountAddress(
            make_child_path(make_child_path(get_path_string(), type), index),
            std::move(address_key)));

    return std::move(result);
}

} // namespace internal
} // namespace wallet_core
