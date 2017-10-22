#include "wallet_core/account.h"

#include "wallet_core/common.h"
#include "wallet_core/error.h"
#include "wallet_core/keys.h"

#include "wallet_core/internal/account_base.h"
#include "wallet_core/internal/bitcoin_account.h"
#include "wallet_core/internal/ethereum_account.h"
#include "wallet_core/internal/key.h"
#include "wallet_core/internal/key_ptr.h"
#include "wallet_core/internal/utility.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_account(const Key* master_key, Currency currency, uint32_t index,
                    Account** new_account)
{
    ARG_CHECK(master_key != nullptr);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(new_account != nullptr);

    try
    {
        switch (currency)
        {
            case CURRENCY_BITCOIN:
            {
                *new_account = new BitcoinAccount(*master_key, index);
                return nullptr;
            }
            case CURRENCY_ETHEREUM:
            {
                *new_account = new EthereumAccount(*master_key, index);
                return nullptr;
            }
            default:
            {
                return make_error(ERROR_GENERAL_ERROR,
                        "Currency not supported yet");
            }
        }
    }
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

Error* get_account_address_key(
        Account* account,
        AddressType address_type,
        uint32_t index,
        Key** out_key)
{
    ARG_CHECK(account != nullptr);
    ARG_CHECK(address_type == ADDRESS_EXTERNAL
            || address_type == ADDRESS_INTERNAL);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(out_key != nullptr);

    try
    {
        const auto& address = account->get_address(address_type, index);
        KeyPtr key(new Key(address.get_key()));
        *out_key = key.release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

Error* get_account_address_string(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address)
{
    ARG_CHECK(account);
    ARG_CHECK(address_type == ADDRESS_EXTERNAL
            || address_type == ADDRESS_INTERNAL);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(out_address);

    try
    {
        const auto& address = account->get_address(address_type, index);
        *out_address = copy_string(address.get_address_string().c_str());
    }
    catch(...)
    {
        return exception_to_error();
    }

    return nullptr;
}

Error* get_account_address_path(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address_path)
{
    ARG_CHECK(account);
    ARG_CHECK(address_type == ADDRESS_EXTERNAL
            || address_type == ADDRESS_INTERNAL);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(out_address_path);

    try
    {
        const auto& address = account->get_address(address_type, index);
        *out_address_path = copy_string(address.get_path_string().c_str());
    }
    catch(...)
    {
        return exception_to_error();
    }

    return nullptr;
}

Error* get_account_currency(Account* account, Currency* out_currency)
{
    ARG_CHECK(account);
    ARG_CHECK(out_currency);

    try
    {
        *out_currency = account->get_currency();
    }
    catch(...)
    {
        return exception_to_error();
    }
    return nullptr;
}

void free_account(Account* account)
{
    delete account;
}
