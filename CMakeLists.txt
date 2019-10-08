cmake_minimum_required (VERSION 3.12)

project(queso CXX)

### Use build/out for install directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/out" CACHE PATH "binary output directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Configure build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SOURCES
    main.cpp
    timer.cpp
    twitch.cpp
    quesoqueue.cpp
    chat.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
