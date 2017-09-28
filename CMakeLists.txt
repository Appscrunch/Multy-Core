cmake_minimum_required(VERSION 3.1)

project(wallet)

add_library(wally STATIC IMPORTED)
set_target_properties(wally PROPERTIES IMPORTED_LOCATION "${WALLY_LIB_PATH}")

add_executable(${PROJECT_NAME}
    main.cpp
    mnemonic.cpp
)
target_include_directories(wallet PRIVATE ./third-party/libwally-core/include/)
target_link_libraries(wallet wally)

target_compile_features(wallet PRIVATE cxx_nullptr)
