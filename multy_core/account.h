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

struct HDAccount;
struct Account;
struct Error;
struct ExtendedKey;
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

enum KeyType
{
    KEY_TYPE_PRIVATE,
    KEY_TYPE_PUBLIC,
};

/** Make an account of given currency with given id.
 *
 * @param master_key - master key, generated from seed.
 * @param currency - currency to use account for.
 * @param index - acccount index
 * @param account - (out) new account
 */
MULTY_CORE_API Error* make_hd_account(
        const ExtendedKey* master_key,
        Currency currency,
        uint32_t index,
        HDAccount** new_account);

/** Make a leaf HD account - the one that has an address and can be paid from/to.
 * @param base_account - base account, for which leaf is generated.
 * @param address_type - type of address for account: internal or external.
 * @param index - index of account within base account.
 * @param new_account - newly created account, must be freed by caller with
 * free_account().
 */
MULTY_CORE_API Error* make_hd_leaf_account(
        const HDAccount* base_account,
        AddressType address_type,
        uint32_t index,
        Account** new_account);

/** Make regular account from private key and currency.
 * @param currency - currency to use account for.
 * @param serialized_private_key - private key for account.
 * @param new_account - newly created account, must be freed by caller with
 * free_account().
 */
MULTY_CORE_API Error* make_account(
        Currency currency,
        const char* serialized_private_key,
        Account** new_account);

MULTY_CORE_API Error* get_account_key(
        const Account* account,
        KeyType key_type,
        Key** out_key);

MULTY_CORE_API Error* get_account_address_string(
        const Account* account,
        const char** out_address);

MULTY_CORE_API Error* get_account_address_path(
        const Account* account,
        const char** out_address_path);

MULTY_CORE_API Error* get_account_currency(
        const Account* account,
        Currency* out_currency);

MULTY_CORE_API void free_hdaccount(HDAccount*);
MULTY_CORE_API void free_account(Account*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_ACCOUNT_H
