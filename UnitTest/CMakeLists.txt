cmake_minimum_required(VERSION 3.10)
project(unitTests)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(gtest REQUIRED)

include_directories(${gtest_source_dir}/googletest/include)


add_executable(${PROJECT_NAME} unitTest.cpp)

add_dependencies(${PROJECT_NAME} gtest boost rapidjson spdlog)

target_link_libraries(${PROJECT_NAME} ${binary_dir}/lib/libgtest.a ${binary_dir}/lib/libgtest_main.a pthread exchangeLibrary)

