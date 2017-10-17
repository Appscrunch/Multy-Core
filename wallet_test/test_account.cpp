#include "account.h"
#include "internal/key.h"
#include "internal/account_base.h"

#include "value_printers.h"

#include "bip39_test_cases.h"
#include "utility.h"

#include "gtest/gtest.h"

#include <string>
#include <memory>

namespace
{
using namespace wallet_core::internal;
using namespace test_utility;

const char* TEST_ADDRESS = "TEST_ADDRESS";
const char* TEST_PATH = "TEST_PATH";

const uint32_t INVALID_INDEX = HARDENED_INDEX_BASE + 1;
const Currency INVALID_CURRENCY = static_cast<Currency>(-1);

Key make_dummy_key()
{
    Key result;
    memset(&result, 0, sizeof(result));
    return result;
}

struct TestAccountAddress : public wallet_core::internal::AccountAddress
{
public:
    using AccountAddress::AccountAddress;

    std::string get_address_string() const override
    {
        return TEST_ADDRESS;
    }
    std::string get_path() const override
    {
        return TEST_PATH;
    }
};

struct TestAccount : public Account
{
    TestAccount()
        : Account(make_dummy_key(), CURRENCY_BITCOIN, 0)
    {}

    AccountAddressPtr make_address(const Key& /*parent_key*/, uint32_t /*index*/)
    {
        KeyPtr key(new Key(make_dummy_key()));
        return AccountAddressPtr(new TestAccountAddress(std::move(key)));
    }
};

} // namespace

Error* get_account_address_key(Account* account, AddressType type, uint32_t index, Key** key);
Error* get_account_address(Account* account, AddressType type, uint32_t index, char** address);
Error* get_account_address_path(Account* account, AddressType type, uint32_t index, char** address_path);
Error* get_account_currency(Account* account, Currency* currency);

GTEST_TEST(AccountInvalidArgs, free_account)
{
    free_account(nullptr);
    GTEST_SUCCEED();
}

GTEST_TEST(AccountInvalidArgs, make_account)
{
    const Key master_key = make_dummy_key();

    auto error = null_unique_ptr<Error>(free_error);
    auto account = null_unique_ptr<Account>(free_account);

    error.reset(make_account(nullptr, CURRENCY_BITCOIN, 0, reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);

    error.reset(make_account(&master_key, INVALID_CURRENCY, 0, reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);

    error.reset(make_account(&master_key, CURRENCY_BITCOIN, INVALID_INDEX, reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);

    error.reset(make_account(&master_key, CURRENCY_BITCOIN, 0, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountInvalidArgs, get_account_address_key)
{
    const Key master_key = make_dummy_key();

    auto error = null_unique_ptr<Error>(free_error);
    KeyPtr key;

    TestAccount account;

    error.reset(get_account_address_key(&account, ADDRESS_EXTERNAL, 0, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);
}
