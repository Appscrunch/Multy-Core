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
const Currency TEST_CURRENCY = CURRENCY_BITCOIN;

const uint32_t INVALID_INDEX = HARDENED_INDEX_BASE + 1;
const Currency INVALID_CURRENCY = static_cast<Currency>(-1);
const AddressType INVALID_ADDRESS = static_cast<AddressType>(-1);

Key make_dummy_key()
{
    Key result;
    memset(&result, 0, sizeof(result));
    return result;
}

struct TestAccountAddress : public wallet_core::internal::AccountAddress
{
public:
    TestAccountAddress(KeyPtr key, std::string address, std::string path)
        : AccountAddress(std::move(key)),
          address(address),
          path(path)
    {}

    std::string get_address_string() const override
    {
        return address;
    }
    std::string get_path() const override
    {
        return path;
    }

    const std::string address;
    const std::string path;
};

struct TestAccount : public Account
{
    TestAccount()
        : TestAccount(make_dummy_key(), TEST_CURRENCY, TEST_ADDRESS, TEST_PATH)
    {}

    TestAccount(Key key, Currency currency, std::string address, std::string path)
        : Account(key, currency, 0),
          key(key),
          address(address),
          path(path)
    {}

    AccountAddressPtr make_address(const Key& /*parent_key*/, uint32_t /*index*/)
    {
        KeyPtr address_key(new Key(key));
        return AccountAddressPtr(new TestAccountAddress(std::move(address_key), address, path));
    }

    const Key key;
    const std::string address;
    const std::string path;
};

} // namespace

GTEST_TEST(AccountTest, free_account)
{
    free_account(nullptr);
    GTEST_SUCCEED();
}

GTEST_TEST(AccountTest, get_account_address_key)
{
    /// TODO: test that only private or public part of the key returned.
    FAIL();
}

GTEST_TEST(AccountTest, fake_account)
{
    const Key expected_key = make_dummy_key();
    const char* EXPECTED_ADDRESS = TEST_ADDRESS;
    const char* EXPECTED_PATH = TEST_PATH;
    const Currency EXPECTED_CURRENCY = CURRENCY_BITCOIN;

    auto error = null_unique_ptr<Error>(free_error);
    TestAccount account(expected_key, EXPECTED_CURRENCY, EXPECTED_ADDRESS, EXPECTED_PATH);

    {
        KeyPtr key;
        error.reset(get_account_address_key(&account, ADDRESS_EXTERNAL, 0, reset_sp(key)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, key);
        ASSERT_EQ(expected_key, *key);
    }

    {
        auto address_str = null_unique_ptr<const char>(free_string);
        error.reset(get_account_address_string(&account, ADDRESS_EXTERNAL, 0, reset_sp(address_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, address_str);
        ASSERT_STREQ(EXPECTED_ADDRESS, address_str.get());
    }

    {
        auto path_str = null_unique_ptr<const char>(free_string);
        error.reset(get_account_address_path(&account, ADDRESS_EXTERNAL, 0, reset_sp(path_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, path_str);
        ASSERT_STREQ(EXPECTED_PATH, path_str.get());
    }

    {
        Currency currency = INVALID_CURRENCY;

        error.reset(get_account_currency(&account, &currency));
        EXPECT_EQ(nullptr, error);
        ASSERT_EQ(EXPECTED_CURRENCY, currency);
    }
}

GTEST_TEST(AccountTestInvalidArgs, make_account)
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

GTEST_TEST(AccountTestInvalidArgs, get_account_address_key)
{
    auto error = null_unique_ptr<Error>(free_error);
    KeyPtr key;

    TestAccount account;

    error.reset(get_account_address_key(nullptr, ADDRESS_EXTERNAL, 0, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(get_account_address_key(&account, INVALID_ADDRESS, 0, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(get_account_address_key(&account, ADDRESS_EXTERNAL, INVALID_INDEX, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(get_account_address_key(&account, ADDRESS_EXTERNAL, 0, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_address_string)
{
    auto error = null_unique_ptr<Error>(free_error);
    auto address_str = null_unique_ptr<const char>(free_string);

    TestAccount account;

    error.reset(get_account_address_string(nullptr, ADDRESS_EXTERNAL, 0, reset_sp(address_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, address_str);

    error.reset(get_account_address_string(&account, INVALID_ADDRESS, 0, reset_sp(address_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, address_str);

    error.reset(get_account_address_string(&account, ADDRESS_EXTERNAL, INVALID_INDEX, reset_sp(address_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, address_str);

    error.reset(get_account_address_string(&account, ADDRESS_EXTERNAL, 0, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_address_path)
{
    auto error = null_unique_ptr<Error>(free_error);
    auto path_str = null_unique_ptr<const char>(free_string);

    TestAccount account;

    error.reset(get_account_address_path(nullptr, ADDRESS_EXTERNAL, 0, reset_sp(path_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, path_str);

    error.reset(get_account_address_path(&account, INVALID_ADDRESS, 0, reset_sp(path_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, path_str);

    error.reset(get_account_address_path(&account, ADDRESS_EXTERNAL, INVALID_INDEX, reset_sp(path_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, path_str);

    error.reset(get_account_address_path(&account, ADDRESS_EXTERNAL, 0, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_currency)
{
    auto error = null_unique_ptr<Error>(free_error);
    Currency currency = INVALID_CURRENCY;

    TestAccount account;

    error.reset(get_account_currency(nullptr, &currency));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(INVALID_CURRENCY, currency);

    error.reset(get_account_currency(&account, nullptr));
    EXPECT_NE(nullptr, error);
}