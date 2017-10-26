/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ACCOUNT_H
#define MULTY_CORE_ACCOUNT_H

#include "multy_core/api.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Account;
struct Error;
struct Key;

enum Currency
{
    CURRENCY_BITCOIN,
    CURRENCY_ETHEREUM,
};

enum AddressType
{
    ADDRESS_EXTERNAL,
    ADDRESS_INTERNAL // change-address
};

/** Make an account of given currency with given id.
 *
 * @param master_key - master key, generated from seed.
 * @param currency - currency to use account for.
 * @param index - acccount index
 * @param account - (out) new account
 */
MULTY_CORE_API Error* make_account(
        const Key* master_key,
        Currency currency,
        uint32_t index,
        Account** new_account);

MULTY_CORE_API Error* get_account_address_key(
        Account* account,
        AddressType address_type,
        uint32_t index,
        Key** out_key);

MULTY_CORE_API Error* get_account_address_string(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address);

MULTY_CORE_API Error* get_account_address_path(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address_path);

MULTY_CORE_API Error* get_account_currency(
        Account* account, Currency* out_currency);

MULTY_CORE_API void free_account(Account*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_ACCOUNT_H
