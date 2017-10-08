#ifndef WALLET_ACCOUNT_BASE_H
#define WALLET_ACCOUNT_BASE_H

#include "account.h"
#include "keys.h"

#include "internal/key_ptr.h"

#include <array>
#include <stddef.h>
#include <memory>
#include <unordered_map>

struct Key;

namespace wallet_core
{
namespace internal
{
// According to bip 44, complete address path looks like:
// m / purpose' / coin_type' / account' / change / address_index
// here account-part path is 3-indices long (m is not part of the path).
const size_t BIP44_ACCOUNT_PATH_DEPTH = 3;

const uint32_t HARDENED_INDEX_BASE = 0x80000000;

class AccountAddress
{
public:
    AccountAddress(KeyPtr private_key, KeyPtr public_key);
    virtual ~AccountAddress();

    virtual std::string get_address() = 0;
    virtual std::string get_path() = 0;

    const KeyPtr private_key;
    const KeyPtr public_key;
};

typedef std::unique_ptr<AccountAddress> AccountAddressPtr;

} // namespace wallet_core

} // namespace internal

// Base class for coin-specific accounts.
// Declared a struct (and out of wallet_core::internal namespace)
// for consitency with a C-like interface.
struct Account
{
    typedef wallet_core::internal::KeyPtr KeyPtr;
    typedef wallet_core::internal::AccountAddress AccountAddress;
    typedef wallet_core::internal::AccountAddressPtr AccountAddressPtr;

    Account(const Key& bip44_master_key, Currency currency, uint32_t index);
    virtual ~Account();

    Currency get_currency() const;
    const AccountAddress& get_address(AddressType type, uint32_t index);

    virtual AccountAddressPtr make_address(const Key& parent_key, uint32_t index) = 0;

private:

    KeyPtr m_account_key;
    const Currency m_currency;
    uint32_t m_bip44_path[wallet_core::internal::BIP44_ACCOUNT_PATH_DEPTH];

    std::array<KeyPtr, 2> m_type_keys;
    std::unordered_map<uint64_t, AccountAddressPtr> m_addresses;
    };

#endif // WALLET_ACCOUNT_BASE_H
