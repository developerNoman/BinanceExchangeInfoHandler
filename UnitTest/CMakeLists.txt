cmake_minimum_required(VERSION 3.10)
project(unitTests)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(${PROJECT_NAME} unitTest.cpp)

add_dependencies(${PROJECT_NAME} gtest boost rapidjson spdlog)

target_link_libraries(${PROJECT_NAME} gtest_main exchangeLibrary pthread)