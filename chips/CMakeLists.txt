project(count CXX)

set(SOURCES
    count.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
spdlog_enable_warnings(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
