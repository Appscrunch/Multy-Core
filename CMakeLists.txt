cmake_minimum_required(VERSION 3.1)

project(wallet)

add_executable(${PROJECT_NAME}
    main.cpp
)
target_link_libraries(${PROJECT_NAME} wallet_core)

target_compile_features(wallet PRIVATE cxx_nullptr)
add_subdirectory(wallet_core)
add_subdirectory(third-party)

