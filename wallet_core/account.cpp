#include "account.h"

#include "common.h"
#include "error.h"
#include "keys.h"

#include "internal/account_base.h"
#include "internal/bitcoin_account.h"
#include "internal/key_ptr.h"
#include "internal/utility.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_account(const Key* master_key, Currency currency, uint32_t index,
                    Account** new_account)
{
    ARG_CHECK(master_key != nullptr);
    ARG_CHECK(new_account != nullptr);
    ARG_CHECK(index < HARDENED_INDEX_BASE);

    try
    {
        switch (currency)
        {
            case CURRENCY_BITCOIN:
                {
                    *new_account = new BitcoinAccount(*master_key, index);
                    return nullptr;
                }
            default:
            {
                return make_error(ERROR_GENERAL_ERROR, "Currency support not implemented yet");
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
        AddressType type,
        uint32_t index,
        Key** key)
{
    ARG_CHECK(account != nullptr);
    ARG_CHECK(key != nullptr);

    try
    {
//        KeyPtr intermediate_key, output_key;
//        throw_if_error(make_child_key(account->account_key.get(),
//                                      CHILD_KEY_TYPE_PRIVATE,
//                                      type,
//                                      reset_sp(intermediate_key)));

//        throw_if_error(make_child_key(intermediate_key.get(),
//                                      CHILD_KEY_TYPE_PRIVATE,
//                                      index,
//                                      reset_sp(output_key)));

//        *key = output_key.release();
//        return nullptr;
    }
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

Error* get_account_address(
        Account* account,
        AddressType type,
        uint32_t index,
        char** address)
{
    ARG_CHECK(account);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(address);

    try
    {
        *address = copy_string(
                account->get_address(type, index).get_address_string().c_str());
    }
    catch(...)
    {
        return exception_to_error();
    }

    return nullptr;
}

Error* get_account_address_path(
        Account* account,
        AddressType type,
        uint32_t index,
        char** address_path)
{
    ARG_CHECK(account);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(address_path);

    try
    {
        *address_path = copy_string(
                account->get_address(type, index).get_path().c_str());
    }
    catch(...)
    {
        return exception_to_error();
    }

    return nullptr;
}

Error* get_account_currency(Account* account, Currency* currency)
{
    ARG_CHECK(account);
    ARG_CHECK(currency);

    *currency = account->get_currency();
    return nullptr;
}

void free_account(Account* account)
{
    delete account;
}
