/* Copyright Multy.io
 * Licensed under Attribution-NonCommercial-NoDerivatives 4.0 International
 * (CC BY-NC-ND 4.0)
 * See LICENSE for details
 */

#ifndef WALLET_TEST_RUN_TESTS_H
#define WALLET_TEST_RUN_TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUILDING_WALLET_TESTS
#    define BUILDING_WALLET_TESTS 0
#endif

#if defined(_WIN32)
#    if (BUILDING_WALLET_TESTS)
#        define WALLET_TESTS_API __declspec(dllexport)
#    else
#        define WALLET_TESTS_API
#    endif
#elif defined(__GNUC__) && (BUILDING_WALLET_TESTS)
#    define WALLET_TESTS_API __attribute__ ((visibility ("default")))
#else
#    define WALLET_TESTS_API
#endif

WALLET_TESTS_API int run_tests(int argc, char **argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_TEST_RUN_TESTS_H
