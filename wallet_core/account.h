#ifndef ACCOUNT_H
#define ACCOUNT_H

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
    CURRENCY_BITCOIN_CASH,
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
Error* make_account(const Key* master_key, Currency currency, uint32_t index,
                    Account** account);

Error* get_account_address_key(Account* account, AddressType type, uint32_t index, Key** key);
Error* get_account_address(Account* account, AddressType type, uint32_t index, char** address);
Error* get_account_address_path(Account* account, AddressType type, uint32_t index, char** address_path);
Error* get_account_currency(Account* account, Currency* currency);

void free_account(Account*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ACCOUNT_H
