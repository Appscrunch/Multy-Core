cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_TESTS "Build tests shared library (DLL)" NO)
set(LIBWALLY_BIN "" CACHE FILEPATH "Path to prebuild libwally-core")
set(LIBWALLY_BIN_PATH "" CACHE PATH "Path to directory with prebuild libwally-core")

project(wallet)

add_executable(
    wallet
    main.cpp
)
target_link_libraries(wallet PRIVATE multy_core)
target_include_directories(wallet PRIVATE .)

add_subdirectory(multy_core)
add_subdirectory(third-party)

if(WITH_TESTS)
    target_compile_definitions(wallet PRIVATE WITH_TESTS)
    add_subdirectory(wallet_test)
    target_link_libraries(wallet PRIVATE wallet_test)
endif()

