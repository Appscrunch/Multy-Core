/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "wallet_core/internal/key.h"
#include "wallet_core/keys.h"

#include "wallet_test/bip39_test_cases.h"
#include "wallet_test/utility.h"
#include "wallet_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>

namespace
{
using namespace wallet_core::internal;
using namespace test_utility;

Key make_dummy_key()
{
    Key result;
    memset(&result, 0, sizeof(result));
    return result;
}

class KeysTestValidCasesP : public ::testing::TestWithParam<BIP39TestCase>
{
};

} // namespace

INSTANTIATE_TEST_CASE_P(
        BIP39,
        KeysTestValidCasesP,
        ::testing::ValuesIn(BIP39_DEFAULT_TEST_CASES));

TEST_P(KeysTestValidCasesP, Test)
{
    const BIP39TestCase& param = GetParam();
    const bytes seed_data = from_hex(param.seed);
    const BinaryData seed{seed_data.data(), seed_data.size()};

    auto error = null_unique_ptr<Error>(free_error);
    auto key = null_unique_ptr<Key>(free_key);
    auto key_string = null_unique_ptr<const char>(free_string);

    error.reset(make_master_key(&seed, reset_sp(key)));
    EXPECT_EQ(nullptr, error);
    ASSERT_NE(nullptr, key);

    error.reset(
            key_to_base58(key.get(), KEY_TYPE_PRIVATE, reset_sp(key_string)));
    EXPECT_EQ(nullptr, error);
    EXPECT_STREQ(param.root_key, key_string.get());
}

GTEST_TEST(KeysTestInvalidArgs, make_master_key)
{
    const unsigned char data_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData data{data_vals, 3};

    auto error = null_unique_ptr<Error>(free_error);
    auto key = null_unique_ptr<Key>(free_key);

    error.reset(make_master_key(nullptr, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(make_master_key(&data, nullptr));
    EXPECT_NE(nullptr, error);

    // Even though all arguments are present, data is still invalid
    error.reset(make_master_key(&data, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);
}

GTEST_TEST(KeysTestInvalidArgs, make_child_key)
{
    const KeyType INVALID_KEY_TYPE = static_cast<KeyType>(-1);
    const Key parent_key = make_dummy_key();

    auto error = null_unique_ptr<Error>(free_error);
    auto child_key = null_unique_ptr<Key>(free_key);

    // Event though all parameters are preset, parent_key is still invalid
    error.reset(
            make_child_key(
                    &parent_key, KEY_TYPE_PRIVATE, 0, reset_sp(child_key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, child_key);

    error.reset(
            make_child_key(nullptr, KEY_TYPE_PRIVATE, 0, reset_sp(child_key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, child_key);

    error.reset(
            make_child_key(
                    &parent_key, INVALID_KEY_TYPE, 0, reset_sp(child_key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, child_key);

    error.reset(make_child_key(&parent_key, KEY_TYPE_PRIVATE, 0, nullptr));
    EXPECT_NE(nullptr, error);
}
