/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef WALLET_CORE_ETHEREUM_ACCOUNT_H
#define WALLET_CORE_ETHEREUM_ACCOUNT_H

#include "wallet_core/internal/account_base.h"

namespace wallet_core
{
namespace internal
{

class EthereumAccount : public ::Account
{
public:
    EthereumAccount(const Key& bip44_master_key, uint32_t index);

protected:
    AccountAddressPtr make_address(
            const Key& parent_key, AddressType type, uint32_t index) override;
};

} // namespace internal
} // namespace wallet_core

#endif // WALLET_CORE_ETHEREUM_ACCOUNT_H
