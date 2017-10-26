/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#ifndef TEST_UTILITY_CPP
#define TEST_UTILITY_CPP

#include "wallet_core/internal/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;

struct A
{
};

static const char* const TEST_VALUE1 = "test value1";
static const char* const TEST_VALUE2 = "test value2";
} // namespace

GTEST_TEST(UtilityTest, null_unique_ptr)
{
    auto p_char = null_unique_ptr<const char>([](const char*) {});
    EXPECT_EQ(nullptr, p_char);

    p_char.reset(TEST_VALUE1);
    EXPECT_EQ(TEST_VALUE1, p_char.get());

    p_char.reset(TEST_VALUE2);
    EXPECT_EQ(TEST_VALUE2, p_char.get());

    auto p_a = null_unique_ptr<A>([](A*){});
    EXPECT_EQ(nullptr, p_a);
}

GTEST_TEST(UtilityTest, reset_sp)
{
    const char* old_value = nullptr;
    auto p_char = null_unique_ptr<const char>([&old_value](const char* to_reset)
    {
        old_value = to_reset;
    });

    auto f = [](const char* new_val, const char** val)
    {
        *val = new_val;
    };

    f(TEST_VALUE1, reset_sp(p_char));
    EXPECT_EQ(nullptr, old_value);
    EXPECT_EQ(TEST_VALUE1, p_char.get());
    p_char.reset();
    EXPECT_EQ(TEST_VALUE1, old_value);

    f(TEST_VALUE2, reset_sp(p_char));
    EXPECT_EQ(TEST_VALUE1, old_value);
    EXPECT_EQ(TEST_VALUE2, p_char.get());

    p_char.reset();
    EXPECT_EQ(TEST_VALUE2, old_value);
}

#endif // TEST_UTILITY_CPP
