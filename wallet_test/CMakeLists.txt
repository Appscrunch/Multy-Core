
add_library(wallet_test SHARED
    run_tests.cpp
    test_account.cpp
    test_keys.cpp
    test_mnemonic.cpp
    test_utility.cpp
    value_printers.cpp
    utility.cpp
)

target_include_directories(
    wallet_test
    PRIVATE
    ..
    ../third-party/googletest/googletest/include
    ../third-party/googletest/googlemock/include
    ../third-party/libwally-core/include
)

target_link_libraries(
    wallet_test
    PRIVATE
    gtest
    wallet_core
)
