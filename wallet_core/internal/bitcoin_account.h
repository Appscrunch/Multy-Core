#ifndef WALLET_CORE_BITCOIN_ACCOUNT_H
#define WALLET_CORE_BITCOIN_ACCOUNT_H

#include "account_base.h"

namespace wallet_core
{
namespace internal
{

class BitcoinAccount : public ::Account
{
public:
    BitcoinAccount(const Key& bip44_master_key, uint32_t index);
    ~BitcoinAccount();

    AccountAddressPtr make_address(const Key& parent_key, AddressType type, uint32_t index) override;
};

} // namespace wallet_core
} // namespace internal

#endif // WALLET_CORE_BITCOIN_ACCOUNT_H
