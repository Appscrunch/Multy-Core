#include "run_tests.h"

#include "gtest/gtest.h"

using namespace std;

int run_tests(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
