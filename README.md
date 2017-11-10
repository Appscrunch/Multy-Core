# Multy-Core
Coming soon..


# For run on Android, past this strings in Multy-core/CMakeList.txt after "add_subdirectory(third-party)"

"""

find_library( # Sets the name of the path variable.
log-lib
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

"""
# For run on iOS:
"""
$ cmake ../Multy-Core  -GXcode -DCMAKE_TOOLCHAIN_FILE=../Multy-Core/tools/ios-cmake/ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_DEPLOYMENT_TARGET=10.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DWITH_TESTS=1
"""
