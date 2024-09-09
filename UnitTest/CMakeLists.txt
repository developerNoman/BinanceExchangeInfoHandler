cmake_minimum_required(VERSION 3.10)
project(unitTests)

find_package(gtest REQUIRED)

add_executable(${PROJECT_NAME} unitTest.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${gtest_source_dir}/googletest/include)

add_dependencies(${PROJECT_NAME} gtest boost rapidjson spdlog)

target_link_libraries(${PROJECT_NAME} ${binary_dir}/lib/libgtest.a pthread exchangeLibrary)
