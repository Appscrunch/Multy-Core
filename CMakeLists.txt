cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER gcc)

option(WITH_TESTS "Build tests shared library (DLL)" NO)
set(LIBWALLY_BIN "" CACHE FILEPATH "Path to prebuild libwally-core")
set(LIBWALLY_BIN_PATH "" CACHE PATH "Path to directory with prebuild libwally-core")
set(LIBSECP256K1_BIN "" CACHE FILEPATH "Path to prebuild libsecp256k1")
set(LIBSECP256K1_BIN_PATH "" CACHE PATH "Path to directory with prebuild libsecp256k1")

project(multy)

add_executable(
        multy
        main.cpp
)
target_link_libraries(multy PRIVATE multy_core)
target_include_directories(multy PRIVATE .)

add_subdirectory(multy_core)
add_subdirectory(third-party)

find_library( # Sets the name of the path variable.
             log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


add_library(core_jni
   SHARED
   /Users/pavel/AndroidStudioProjects/Multi/app/src/main/cpp/scratch.cpp
)
target_link_libraries(core_jni PRIVATE multy_core multy_test android ${log-lib})


target_link_libraries(core_jni PRIVATE multy_core)
target_include_directories(core_jni PRIVATE .)
set_target_properties(core_jni PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
    LANGUAGE CXX
)
set_source_files_properties(scratch.cpp PROPERTIES LANGUAGE CXX)


if(WITH_TESTS)
    target_compile_definitions(multy PRIVATE WITH_TESTS)
    add_subdirectory(multy_test)
    target_link_libraries(multy PRIVATE multy_test)
endif()

