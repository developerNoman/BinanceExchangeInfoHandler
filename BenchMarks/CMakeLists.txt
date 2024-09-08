cmake_minimum_required(VERSION 3.10)
project(BenchMark)

find_package(benchmark REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(${PROJECT_NAME} benchmark.cpp)

add_dependencies(${PROJECT_NAME} benchmark boost)
target_link_libraries(${PROJECT_NAME} ${binary_dir}/src/libbenchmark.a pthread exchangeLibrary)