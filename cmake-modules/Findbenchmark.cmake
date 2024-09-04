include(ExternalProject)

ExternalProject_Add(
    benchmark
    PREFIX ${CMAKE_BINARY_DIR}/benchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.0.tar.gz
    CMAKE_ARGS -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release    
    INSTALL_COMMAND ""    
)

ExternalProject_Get_Property(benchmark binary_dir)