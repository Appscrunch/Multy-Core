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
