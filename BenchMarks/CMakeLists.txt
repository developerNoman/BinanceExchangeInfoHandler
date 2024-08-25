include(ExternalProject)

ExternalProject_Add(benchmark
    PREFIX ${CMAKE_BINARY_DIR}/benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.8.5
    CMAKE_ARGS -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release
)

ExternalProject_Get_Property(benchmark source_dir binary_dir)

add_library(benchmark_main STATIC IMPORTED)
set_target_properties(benchmark_main PROPERTIES
    IMPORTED_LOCATION ${binary_dir}/src/libbenchmark.a
    INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/include
)

add_executable(my_benchmark benchmark.cpp)

target_link_libraries(my_benchmark PRIVATE benchmark_main pthread ${OPENSSL_LIBRARIES})


