project (SimpleJSON CXX)

set(SOURCES
    JSONValue.cpp
    JSON.cpp
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
spdlog_enable_warnings(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)