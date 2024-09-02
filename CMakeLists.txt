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

# add_executable(exchangeHandler
#  app/main.cpp 
#  src/fetchData.cpp
#  src/processQueries.cpp
#  )
# target_link_libraries(exchangeHandler PRIVATE ${OPENSSL_LIBRARIES})
link_libraries(${OPENSSL_LIBRARIES})

add_subdirectory(external/gtest)

add_subdirectory(src)
add_subdirectory(app)
# add_subdirectory(benchmark)
# add_subdirectory(unittest)

# add_subdirectory(UnitTest)
# add_subdirectory(BenchMarks)

