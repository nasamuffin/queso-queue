cmake_minimum_required (VERSION 3.10)

### Configure build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(queso CXX)

add_subdirectory(3rdparty)
add_subdirectory(chips)

### Use build/out for install directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/out" CACHE PATH "binary output directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory \
    (or any directory with a CMakeLists.txt file). Please make a build subdirectory.\
    Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Check for cURL
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

set(SOURCES
    main.cpp
    timer.cpp
    twitch.cpp
    quesoqueue.cpp
    chat.cpp
    chips.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

include_directories(${CURL_INCLUDE_DIR})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
spdlog_enable_warnings(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES} spdlog::spdlog dl SimpleJSON)
