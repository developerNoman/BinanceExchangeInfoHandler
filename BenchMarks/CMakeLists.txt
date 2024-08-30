include(ExternalProject)

ExternalProject_Add(benchmark
    PREFIX ${CMAKE_BINARY_DIR}/benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.8.5
    CMAKE_ARGS -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release    
    INSTALL_COMMAND ""    
)

ExternalProject_Get_Property(benchmark binary_dir)
find_package(OpenSSL REQUIRED)

add_executable(my_benchmark benchmark.cpp)

add_dependencies(my_benchmark benchmark boost)

target_link_libraries(my_benchmark ${binary_dir}/src/libbenchmark.a pthread ${OPENSSL_LIBRARIES})