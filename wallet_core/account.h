#ifndef WALLET_CORE_ACCOUNT_H
#define WALLET_CORE_ACCOUNT_H

#include "wallet_core/api.h"

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
WALLET_CORE_API Error* make_account(
        const Key* master_key,
        Currency currency,
        uint32_t index,
        Account** new_account);

WALLET_CORE_API Error* get_account_address_key(
        Account* account,
        AddressType address_type,
        uint32_t index,
        Key** out_key);

WALLET_CORE_API Error* get_account_address_string(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address);

WALLET_CORE_API Error* get_account_address_path(
        Account* account,
        AddressType address_type,
        uint32_t index,
        const char** out_address_path);

WALLET_CORE_API Error* get_account_currency(
        Account* account,
        Currency* out_currency);

WALLET_CORE_API void free_account(Account*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_CORE_ACCOUNT_H
