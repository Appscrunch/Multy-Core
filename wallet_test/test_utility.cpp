#ifndef TEST_UTILITY_CPP
#define TEST_UTILITY_CPP

#include "wallet_core/internal/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;

struct A
{};
} // namespace

GTEST_TEST(UtilityTest, null_unique_ptr)
{
    auto p_char = null_unique_ptr<char>([](char*){});
    EXPECT_EQ(nullptr, p_char);

    auto p_a = null_unique_ptr<A>([](A*){});
    EXPECT_EQ(nullptr, p_a);
}

#endif // TEST_UTILITY_CPP
