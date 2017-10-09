#ifndef WALLET_ACCOUNT_BASE_H
#define WALLET_ACCOUNT_BASE_H

#include "account.h"
#include "keys.h"

#include "internal/key_ptr.h"

#include <array>
#include <memory>
#include <stddef.h>
#include <unordered_map>
#include <vector>

struct Key;

namespace wallet_core
{
namespace internal
{
// Hierarchical Deterministic path (bip32, bip44).
typedef std::vector<uint32_t> HDPath;

const uint32_t HARDENED_INDEX_BASE = 0x80000000;

class AccountAddress
{
public:
    AccountAddress(KeyPtr extended_key);
    AccountAddress(const HDPath& path, KeyPtr extended_key);
    virtual ~AccountAddress();

    virtual std::string get_address_string() const = 0;
    virtual std::string get_path() const = 0;
    //virtual std::string get_key() const = 0;

    const HDPath path;
    const KeyPtr extended_key;
};

typedef std::unique_ptr<AccountAddress> AccountAddressPtr;

} // namespace wallet_core

} // namespace internal

// Base class for coin-specific accounts.
// Declared a struct (and out of wallet_core::internal namespace)
// for consitency with a C-like interface.
struct Account
{
public:
    typedef wallet_core::internal::KeyPtr KeyPtr;
    typedef wallet_core::internal::AccountAddress AccountAddress;
    typedef wallet_core::internal::AccountAddressPtr AccountAddressPtr;
    typedef wallet_core::internal::HDPath HDPath;

    Account(const Key& bip44_master_key, Currency currency, uint32_t index);
    virtual ~Account();

    Currency get_currency() const;
    const AccountAddress& get_address(AddressType type, uint32_t index);
    const HDPath& get_path() const;

protected:
    virtual AccountAddressPtr make_address(const Key& parent_key, uint32_t index) = 0;

private:

    KeyPtr m_account_key;
    const Currency m_currency;
    HDPath m_bip44_path;

    std::array<KeyPtr, 2> m_type_keys;
    std::unordered_map<uint64_t, AccountAddressPtr> m_addresses;
};

#endif // WALLET_ACCOUNT_BASE_H