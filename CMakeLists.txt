# cmake_minimum_required(VERSION 3.10)
# project(BinanceExchangeHandler)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


# find_package(OpenSSL REQUIRED)
# include_directories(${OPENSSL_INCLUDE_DIR})

# include_directories(include)

# include(cmake-modules/spdlog.cmake)
# include(cmake-modules/rapidjson.cmake)
# include(cmake-modules/boost.cmake)

# add_executable(exchangeHandler app/data.cpp)
# target_link_libraries(exchangeHandler PRIVATE ${OPENSSL_LIBRARIES})

# add_subdirectory(UnitTest)
# add_subdirectory(BenchMarks)
cmake_minimum_required(VERSION 3.10)
project(BinanceExchangeHandler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/")
find_package(spdlog)
find_package(rapidjson)
find_package(boost)

add_executable(exchangeHandler app/data.cpp)
target_link_libraries(exchangeHandler PRIVATE ${OPENSSL_LIBRARIES})

add_subdirectory(UnitTest)
add_subdirectory(BenchMarks)

add_dependencies(exchangeHandler boost)
