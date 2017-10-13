#include "mnemonic.h"
#include "internal/utility.h"
#include "wally_core.h"

#include "gtest/gtest.h"

#include <string>
#include <memory>
#include <vector>

namespace
{
using namespace wallet_core::internal;
typedef std::vector<unsigned char> bytes;

#define E(statement) throw_if_wally_error((statement), #statement)

size_t dummyEntropySource(size_t /*size*/, void* /*dest*/)
{
    return 0;
}

//bytes from_hex(const char* hex_str)
//{
//    const size_t expected_size = strlen(hex_str)/2;
//    bytes result(expected_size);
//    size_t bytes_written = 0;

//    E(wally_hex_to_bytes(hex_str, result.data(), result.size(), &bytes_written));
//    result.resize(bytes_written);

//    return result;
//}

//struct MnemonicTestCase
//{
//    const bytes entropy;
//    const bytes expected_mnemonic;
//    const bytes expected_seed;

//    MnemonicTestCase(const char* entropy,
//            const char* expected_mnemonic,
//            const char* expected_seed)
//      : entropy(from_hex(entropy)),
//        expected_mnemonic(from_hex(expected_mnemonic)),
//        expected_seed(from_hex(expected_seed))
//    {}
//};

//class ValidMnemonicTest : public ::testing::TestWithParam<MnemonicTestCase> {
////protected:
////    void SetUp() override
////    {
////    }
//};

} // namespace

//TEST_P(ValidMnemonicTest, Test)
//{
//    const auto& param = GetParam();
//    auto mnemonic_str = null_unique_ptr<char>(free_string);
//    //make_mnemonic()
//}

template <typename T>
bool operator==(const std::nullptr_t& n, const std::unique_ptr<T>& t)
{
    return n == t.get();
}

template <typename T>
bool operator!=(const std::nullptr_t& n, const std::unique_ptr<T>& t)
{
    return !(n == t);
}

GTEST_TEST(MnemonicInvalidArgs, make_mnemonic)
{
    auto mnemonic_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(make_mnemonic(nullptr, reset_sp(mnemonic_str)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, mnemonic_str.get());

    error.reset(make_mnemonic(dummyEntropySource, nullptr));
    ASSERT_NE(nullptr, error.get());
}

GTEST_TEST(MnemonicInvalidArgs, make_seed)
{
    auto binary_data = null_unique_ptr<BinaryData>(free_binarydata);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(make_seed(nullptr, "pass", reset_sp(binary_data)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, binary_data.get());

    error.reset(make_seed("mnemonic", nullptr, reset_sp(binary_data)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, binary_data.get());

    error.reset(make_seed("mnemonic", "pass", nullptr));
    ASSERT_NE(nullptr, error.get());

    // Invalid mnemonic value
    error.reset(make_seed("mnemonic", "pass", reset_sp(binary_data)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, binary_data.get());
}

GTEST_TEST(MnemonicInvalidArgs, seed_to_string)
{
    unsigned char data_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData data {data_vals, 3};
    const BinaryData null_data {nullptr, 0};
    const BinaryData zero_len_data {nullptr, 0};

    auto seed_str = null_unique_ptr<const char>(free_string);
    auto error = null_unique_ptr<Error>(free_error);

    error.reset(seed_to_string(nullptr, reset_sp(seed_str)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, seed_str.get());

    error.reset(seed_to_string(&data, nullptr));
    ASSERT_NE(nullptr, error.get());

    error.reset(seed_to_string(&null_data, reset_sp(seed_str)));
    ASSERT_NE(nullptr, error.get());
    ASSERT_EQ(nullptr, seed_str.get());

    error.reset(seed_to_string(&zero_len_data, reset_sp(seed_str)));
    ASSERT_NE(nullptr, error);
    ASSERT_EQ(nullptr, seed_str);
}
