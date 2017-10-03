#include "account.h"

#include "common.h"
#include "error.h"
#include "keys.h"

#include "internal/utility.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;
struct KeyDeleter
{
    void operator()(Key* key)
    {
        free_key(key);
    }
};

const uint32_t BIP44_PURPOSE_CHAIN_CODE = 0x8000002C;
const uint32_t HARDENED_INDEX_BASE = 0x80000000;

uint32_t hardened_index(uint32_t index)
{
    return index | HARDENED_INDEX_BASE;
}

typedef std::unique_ptr<Key, KeyDeleter> KeyPtr;
} // namepace

struct Account
{
    KeyPtr account_key;
    Currency currency;
};

Error* make_account(const Key* master_key, Currency currency, uint32_t index,
                    Account** new_account)
{
    ARG_CHECK(master_key != nullptr);
    ARG_CHECK(new_account != nullptr);
    ARG_CHECK(index < HARDENED_INDEX_BASE);

    KeyPtr purpose_key, currency_key, account_key;
    try
    {
        throw_if_error(make_child_key(master_key, CHILD_KEY_TYPE_PRIVATE,
                                      BIP44_PURPOSE_CHAIN_CODE,
                                      reset_sp(purpose_key)));
        throw_if_error(make_child_key(purpose_key.get(),
                                      CHILD_KEY_TYPE_PRIVATE,
                                      hardened_index(static_cast<uint32_t>(currency)),
                                      reset_sp(currency_key)));
        throw_if_error(make_child_key(currency_key.get(),
                                      CHILD_KEY_TYPE_PRIVATE,
                                      hardened_index(index),
                                      reset_sp(account_key)));
        std::unique_ptr<Account> account(new Account);
        account->account_key.swap(account_key);
        account->currency = currency;
        *new_account = account.release();

        return nullptr;
    }
    catch (Error* e)
    {
        return e;
    }
    catch (...)
    {
        return make_error(ERROR_GENERAL_ERROR, "Unknown error");
    }
    return nullptr;
}

Error* get_account_key(Account* account, AddressType type, uint32_t index, Key** key)
{
    ARG_CHECK(account != nullptr);
    ARG_CHECK(key != nullptr);

    KeyPtr intermediate_key, output_key;
    try
    {
        throw_if_error(make_child_key(account->account_key.get(),
                                      CHILD_KEY_TYPE_PRIVATE,
                                      type,
                                      reset_sp(intermediate_key)));

        throw_if_error(make_child_key(intermediate_key.get(),
                                      CHILD_KEY_TYPE_PRIVATE,
                                      index,
                                      reset_sp(output_key)));

        *key = output_key.release();
        return nullptr;
    }
    catch (Error* e)
    {
        return e;
    }
    catch (...)
    {
        return make_error(ERROR_GENERAL_ERROR, "Unknown error");
    }
    return nullptr;
}

Error* get_account_address(Account* account, AddressType type, uint32_t index, char** address)
{
    ARG_CHECK(account);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(address);

    return make_error(ERROR_GENERAL_ERROR, "Not supported yet");
}

Error* get_account_currency(Account* account, Currency* currency)
{
    ARG_CHECK(account);
    ARG_CHECK(currency);

    *currency = account->currency;
    return nullptr;
}

void free_account(Account* account)
{
    delete account;
}
