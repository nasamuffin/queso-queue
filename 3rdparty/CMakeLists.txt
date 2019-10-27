find_package(Git)

if (GIT_FOUND)
execute_process(COMMAND ${GIT_EXECUTABLE} clone -b "release-1.10.0" "https://github.com/google/googletest.git" -c advice.detachedhead=false  ${CMAKE_BINARY_DIR}/extern/gtest.1.10.0)
execute_process(COMMAND ${GIT_EXECUTABLE} clone -b "v1.4.2" "https://github.com/gabime/spdlog.git" -c advice.detachedhead=false ${CMAKE_BINARY_DIR}/extern/spdlog.1.4.2)
add_subdirectory(${CMAKE_BINARY_DIR}/extern/spdlog.1.4.2 ${CMAKE_BINARY_DIR}/3rdparty/spdlog)
add_subdirectory(${CMAKE_BINARY_DIR}/extern/gtest.1.10.0/googletest ${CMAKE_BINARY_DIR}/3rdparty/gtest)
endif()

add_subdirectory(SimpleJSON)
