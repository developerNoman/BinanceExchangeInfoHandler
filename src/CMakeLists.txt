
file(GLOB LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(exchangeLibrary STATIC ${LIB_SOURCES})

find_package(spdlog REQUIRED)
find_package(rapidjson REQUIRED)
find_package(boost REQUIRED)

add_dependencies(exchangeLibrary rapidjson boost spdlog)
target_include_directories(exchangeLibrary PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(exchangeLibrary PUBLIC ${boost_source_dir}/libs/beast/include)
target_include_directories(exchangeLibrary PUBLIC ${boost_source_dir}/libs/asio/include)
target_include_directories(exchangeLibrary PUBLIC ${boost_source_dir}/libs/beast)
target_include_directories(exchangeLibrary PUBLIC ${spdlog_source_dir})
target_include_directories(exchangeLibrary PUBLIC ${rapidjson_source_dir})