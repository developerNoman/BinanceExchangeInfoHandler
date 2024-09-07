include(ExternalProject)

ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)
set(gtest_source_dir ${source_dir})


