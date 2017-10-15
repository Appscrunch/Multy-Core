cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wallet)

add_executable(
    wallet
    main.cpp
)
target_link_libraries(wallet PRIVATE wallet_core)

add_subdirectory(wallet_core)
add_subdirectory(third-party)

if(DEFINED WITH_TESTS)
    target_compile_definitions(wallet PRIVATE WITH_TESTS)

    target_sources(
        wallet
        PRIVATE
        ./wallet_test/run_tests.cpp
        ./wallet_test/test_mnemonic.cpp
        ./wallet_test/test_utility.cpp
        ./wallet_test/value_printers.cpp
        ./wallet_test/utility.cpp
    )

    target_include_directories(
        wallet
        PRIVATE
        wallet_core
        third-party/googletest/googletest/include
        third-party/googletest/googlemock/include
        third-party/libwally-core/include
    )

    target_link_libraries(
        wallet
        PRIVATE
        gtest
    )
endif(DEFINED WITH_TESTS)

