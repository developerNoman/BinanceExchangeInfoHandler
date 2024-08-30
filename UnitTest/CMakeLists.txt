cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeInfoHandler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories(include)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/external/gtest)
add_subdirectory(${GOOGLETEST_DIR} ${CMAKE_BINARY_DIR}/googletest)

include_directories(${GOOGLETEST_DIR}/googletest/include)
include_directories(${CMAKE_SOURCE_DIR}/include) 

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(runTests unitTest.cpp)
add_dependencies(runTests boost)
target_link_libraries(runTests gtest gtest_main pthread ${OPENSSL_LIBRARIES})

