cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeInfoHandler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIB_SOURCES fetchData.cpp processQueries.cpp)
add_library(exchangeLibrary STATIC ${LIB_SOURCES})
add_dependencies(exchangeLibrary rapidjson boost spdlog)
target_include_directories(exchangeLibrary PUBLIC ${CMAKE_SOURCE_DIR}/include)

ExternalProject_Get_Property(boost source_dir)

target_include_directories(exchangeLibrary PUBLIC ${source_dir}/libs/beast/include)
target_include_directories(exchangeLibrary PUBLIC ${source_dir}/libs/asio/include)
target_include_directories(exchangeLibrary PUBLIC ${source_dir}/libs/beast)