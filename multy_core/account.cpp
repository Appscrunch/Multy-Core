/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/keys.h"

#include "multy_core/internal/account_base.h"
#include "multy_core/internal/bitcoin_account.h"
#include "multy_core/internal/ethereum_account.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/key_ptr.h"
#include "multy_core/internal/utility.h"

#include <memory>
#include <string>

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_hd_account(
        const Key* master_key,
        Currency currency,
        uint32_t index,
        HDAccount** new_account)
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
                *new_account = new BitcoinHDAccount(*master_key, index);
                return nullptr;
            }
            case CURRENCY_ETHEREUM:
            {
                *new_account = new EthereumHDAccount(*master_key, index);
                return nullptr;
            }
            default:
            {
                return make_error(
                        ERROR_GENERAL_ERROR, "Currency not supported yet");
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
        *out_key = account->get_key().release();
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
        *out_address = copy_string(account->get_address());
    }
    catch (...)
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
        *out_address_path = copy_string(to_string(account->get_path()));
    }
    catch (...)
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
    catch (...)
    {
        return exception_to_error();
    }
    return nullptr;
}

void free_account(HDAccount* account)
{
    delete account;
}
