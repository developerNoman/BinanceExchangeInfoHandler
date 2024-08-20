cmake_minimum_required(VERSION 3.30.1)
project(BinanceExchangeInfoHandler)

include(cmake-modules/spdlog.cmake)
include(cmake-modules/rapidjson.cmake)

add_executable(main app/main.cpp)