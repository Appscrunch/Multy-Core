/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ACCOUNT_BASE_H
#define MULTY_CORE_ACCOUNT_BASE_H

#include "multy_core/api.h"
#include "multy_core/account.h"
#include "multy_core/keys.h"

#include "multy_core/internal/account.h"
#include "multy_core/internal/key_ptr.h"
#include "multy_core/internal/hd_path.h"

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

class AccountBase : public Account
{
public:
    typedef wallet_core::internal::KeyPtr KeyPtr;
    typedef wallet_core::internal::HDPath HDPath;

    AccountBase(
            Currency currency,
            KeyPtr extended_key,
            const HDPath& path = HDPath());
    virtual ~AccountBase();

    virtual Currency get_currency() const;
    virtual HDPath get_path() const;
//    virtual std::string get_address() const;
//    virtual KeyPtr get_key() const;

private:
    const Currency m_currency;
    const KeyPtr m_extended_key;
    const HDPath m_path;
};

typedef std::unique_ptr<Account> AccountPtr;

} // namespace wallet_core
} // namespace internal

// Base class for coin-specific HD accounts.
// Declared a struct (and out of wallet_core::internal namespace)
// for consitency with a C-like interface.
struct MULTY_CORE_API HDAccount
{
public:
    typedef wallet_core::internal::AccountPtr AccountPtr;
    typedef wallet_core::internal::KeyPtr KeyPtr;
    typedef wallet_core::internal::HDPath HDPath;

    virtual ~HDAccount();

    AccountPtr make_leaf_account(AddressType type, uint32_t index) const;

protected:
    HDAccount(const Key& bip44_master_key, Currency currency, uint32_t index);

    virtual const HDPath& get_path() const;
    virtual AccountPtr make_account(
            const Key& parent_key,
            AddressType type,
            uint32_t index) const = 0;

private:
    KeyPtr m_account_key;
    const Currency m_currency;
    HDPath m_bip44_path;
};

#endif // MULTY_CORE_ACCOUNT_BASE_H
