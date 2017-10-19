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

target_compile_definitions(wallet_core PRIVATE BUILDING_WALLET_CORE=1)
target_include_directories(wallet_core PRIVATE
    ..
    ../third-party/libwally-core/include/
)
target_link_libraries(wallet_core libwally-core)

