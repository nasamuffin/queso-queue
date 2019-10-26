add_subdirectory(spdlog)
add_subdirectory(SimpleJSON)

find_package(Git)

if (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} clone -b "release-1.10.0" "https://github.com/google/googletest.git" ${CMAKE_BINARY_DIR}/extern/gtest.1.10.0)
    add_subdirectory(${CMAKE_BINARY_DIR}/extern/gtest.1.10.0/googletest ${CMAKE_BINARY_DIR}/3rdparty/)
endif()
