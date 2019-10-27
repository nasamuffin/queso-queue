enable_testing()

set (SOURCES
     timer_test.cpp)

set (UNIT_TEST ${PROJECT_NAME}_test)

add_executable(${UNIT_TEST}
               ${SOURCES})

target_link_libraries(${UNIT_TEST} gtest)

add_test(NAME ${UNIT_TEST}
         COMMAND ${UNIT_TEST})

add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run tests"
     POST_BUILD
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND}
)
