cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeHandler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/")

link_libraries(${OPENSSL_LIBRARIES})

add_subdirectory(src)
add_subdirectory(app)

option(BUILD_TEST "Build tests" ON)

option(BUILD_BENCHMARK "Build benchmarks" ON)

option(BUILD_BOTH "Build tests and benchmarks" ON)


if(BUILD_BOTH)
    if(BUILD_TEST)
        add_subdirectory(UnitTest)
    endif()

    if(BUILD_BENCHMARK)
        add_subdirectory(BenchMarks)
    endif()
endif()



