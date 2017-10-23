/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#include "wallet_core/mnemonic.h"

#include "wallet_test/bip39_test_cases.h"
#include "wallet_test/utility.h"
#include "wallet_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <vector>

namespace
{
using namespace wallet_core::internal;
using namespace test_utility;
typedef std::vector<unsigned char> bytes;

size_t dummy_fill_entropy(void*, size_t size, void* dest)
{
    static const size_t entropy_max_size = 1024;
    unsigned char silly_entropy[entropy_max_size];

    if (size > entropy_max_size)
    {
        return 0;
    }

    memcpy(dest, silly_entropy, size);
    return size;
}

static const EntropySource dummy_entropy_source{nullptr, dummy_fill_entropy};

struct MnemonicTestCase
{
    const bytes entropy;
    const std::string expected_mnemonic;
    const bytes expected_seed;

    MnemonicTestCase(
            const char* entropy,
            const char* expected_mnemonic,
            const char* expected_seed)
        : entropy(from_hex(entropy)),
          expected_mnemonic(expected_mnemonic),
          expected_seed(from_hex(expected_seed))
    {
    }

    MnemonicTestCase(const BIP39TestCase& bip39_test_case)
        : MnemonicTestCase(
                  bip39_test_case.entropy,
                  bip39_test_case.mnemonic,
                  bip39_test_case.seed)
    {
    }
};

class MnemonicTestValidCasesP : public ::testing::TestWithParam<BIP39TestCase>
{
};

} // namespace

INSTANTIATE_TEST_CASE_P(
        BIP39,
        MnemonicTestValidCasesP,
        ::testing::ValuesIn(BIP39_DEFAULT_TEST_CASES));

TEST_P(MnemonicTestValidCasesP, Test)
{
    const BIP39TestCase& param = GetParam();
    const bytes entropy(from_hex(param.entropy));
    const std::string expected_mnemonic(param.mnemonic);
    const bytes expected_seed(from_hex(param.seed));

    auto mnemonic_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    ASSERT_EQ(nullptr, mnemonic_str);
    ASSERT_EQ(nullptr, error);

    auto fill_entropy = [](void* data, ::size_t size, void* dest) -> ::size_t {
        const bytes* entropy = (const bytes*)(data);
        const size_t result_size = std::min(size, entropy->size());
        memcpy(dest, entropy->data(), result_size);
        return result_size;
    };
    auto entropy_source = EntropySource{(void*)&entropy, fill_entropy};
    error.reset(make_mnemonic(entropy_source, reset_sp(mnemonic_str)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, mnemonic_str);

    EXPECT_STREQ(expected_mnemonic.c_str(), mnemonic_str.get());

    auto seed = null_unique_ptr<BinaryData>(free_binarydata);
    error.reset(make_seed(expected_mnemonic.c_str(), "TREZOR", reset_sp(seed)));
    ASSERT_NE(nullptr, seed);
    EXPECT_EQ(to_binary_data(expected_seed), *seed);
}

GTEST_TEST(MnemonicTest, empty_null_password)
{
    auto mnemonic_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(make_mnemonic(dummy_entropy_source, reset_sp(mnemonic_str)));
    ASSERT_NE(nullptr, mnemonic_str);

    auto empty_pass_seed = null_unique_ptr<BinaryData>(free_binarydata);
    error.reset(make_seed(mnemonic_str.get(), "", reset_sp(empty_pass_seed)));
    ASSERT_NE(nullptr, empty_pass_seed);

    auto null_pass_seed = null_unique_ptr<BinaryData>(free_binarydata);
    error.reset(
            make_seed(mnemonic_str.get(), nullptr, reset_sp(null_pass_seed)));
    ASSERT_NE(nullptr, null_pass_seed);

    ASSERT_EQ(*null_pass_seed, *empty_pass_seed);
}

GTEST_TEST(MnemonicTestInvalidArgs, make_mnemonic)
{
    auto mnemonic_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(
            make_mnemonic(
                    EntropySource{nullptr, nullptr}, reset_sp(mnemonic_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, mnemonic_str);

    error.reset(make_mnemonic(dummy_entropy_source, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(MnemonicTestInvalidArgs, make_seed)
{
    auto binary_data = null_unique_ptr<BinaryData>(free_binarydata);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(make_seed(nullptr, "pass", reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);

    error.reset(make_seed("mnemonic", nullptr, reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);

    error.reset(make_seed("mnemonic", "pass", nullptr));
    EXPECT_NE(nullptr, error);

    // Invalid mnemonic value
    error.reset(make_seed("mnemonic", "pass", reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);
}

GTEST_TEST(MnemonicTestInvalidArgs, seed_to_string)
{
    unsigned char data_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData data{data_vals, 3};
    const BinaryData null_data{nullptr, 0};
    const BinaryData zero_len_data{nullptr, 0};

    auto seed_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(seed_to_string(nullptr, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);

    error.reset(seed_to_string(&data, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(seed_to_string(&null_data, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);

    error.reset(seed_to_string(&zero_len_data, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);
}
