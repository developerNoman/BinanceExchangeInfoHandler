cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(include)

include(cmake-modules/spdlog.cmake)
include(cmake-modules/rapidjson.cmake)
include(cmake-modules/boost.cmake)

add_executable(main app/data.cpp)
target_link_libraries(main PRIVATE ${OPENSSL_LIBRARIES})

add_subdirectory(UnitTest)
add_subdirectory(BenchMarks)