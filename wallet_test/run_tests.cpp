/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#define BUILDING_WALLET_TESTS 1
#include "wallet_test/run_tests.h"

#include "gtest/gtest.h"

int run_tests(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
