cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeHandler)

add_executable(my_unitTests unitTest.cpp)

add_dependencies(my_unitTests gtest boost rapidjson spdlog)

target_link_libraries(my_unitTests gtest exchangeLibrary)