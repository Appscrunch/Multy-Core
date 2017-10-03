add_library(wally SHARED IMPORTED)
set_target_properties(wally PROPERTIES IMPORTED_LOCATION "${WALLY_LIB_PATH}")

add_library(wallet_core
    SHARED
    account.cpp
    common.cpp
    error.cpp
    keys.cpp
    mnemonic.cpp
)

target_include_directories(wallet_core PRIVATE ../third-party/libwally-core/include/ .)
target_link_libraries(wallet_core wally)
target_compile_features(wallet_core PRIVATE cxx_nullptr)
