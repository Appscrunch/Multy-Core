#include "bitcoin_account.h"

#include "internal/utility.h"

#include "wally_crypto.h"

namespace
{
using namespace wallet_core::internal;
struct BitcoinAddress : public AccountAddress
{
    // using parent constructor
    using AccountAddress::AccountAddress;

    std::string get_address() override
    {
//        std::string address(27, '\0');
//        wally_hash160( address.get(), HASH160_LEN);
        return "not implremented yet";
    }
    std::string get_path() override
    {
        return "not implremented yet";
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
    KeyPtr address_private_key, address_public_key;

    throw_if_error(make_child_key(&parent_key,
                                  CHILD_KEY_TYPE_PRIVATE,
                                  index,
                                  reset_sp(address_private_key)));

    throw_if_error(make_child_key(&parent_key,
                                  CHILD_KEY_TYPE_PUBLIC,
                                  index,
                                  reset_sp(address_public_key)));

    return std::unique_ptr<AccountAddress>(new BitcoinAddress(
            std::move(address_private_key), std::move(address_public_key)));
}

} // namespace wallet_core
} // namespace internal
