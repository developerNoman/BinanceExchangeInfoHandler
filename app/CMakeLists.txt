cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeInfoHandler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} main.cpp)

add_dependencies(${PROJECT_NAME} spdlog rapidjson boost)

target_link_libraries(${PROJECT_NAME} exchangeLibrary)