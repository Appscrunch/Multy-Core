/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_ACCOUNT_H
#define MULTY_CORE_ETHEREUM_ACCOUNT_H

#include "multy_core/internal/account_base.h"

namespace wallet_core
{
namespace internal
{

class EthereumHDAccount : public ::HDAccount
{
public:
    EthereumHDAccount(const Key& bip44_master_key, uint32_t index);

protected:
    AccountPtr make_account(
            const Key& parent_key, AddressType type, uint32_t index) const override;
};

} // namespace internal
} // namespace wallet_core

#endif // MULTY_CORE_ETHEREUM_ACCOUNT_H
