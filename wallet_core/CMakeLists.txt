set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(wallet_core
    SHARED
    account.cpp
    common.cpp
    error.cpp
    keys.cpp
    mnemonic.cpp
    internal/account_base.cpp
    internal/bitcoin_account.cpp
    internal/utility.cpp
)

target_include_directories(wallet_core PRIVATE ../third-party/libwally-core/include/ .)
target_link_libraries(wallet_core wally)

target_include_directories(wallet_core PRIVATE ../third-party/libwally-core/include/ .)
if (DEFINED LIBWALLY_BIN_PATH)
    # wally is prebuild
    add_library(wally SHARED IMPORTED)
    if (DEFINED ANDROID_ABI)
        set_target_properties(wally PROPERTIES IMPORTED_LOCATION
            "${LIBWALLY_BIN_PATH}/${ANDROID_ABI}/libwallycore.so")
    else()
        set_target_properties(wally PROPERTIES IMPORTED_LOCATION
            "${LIBWALLY_BIN_PATH}/libwallycore.so")
    endif()
else()
    add_subdirectory(third-party)
endif()
