include(ExternalProject)

ExternalProject_Add(
    rapidjson
    PREFIX ${CMAKE_BINARY_DIR}/rapidjson
    URL https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.tar.gz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

 ExternalProject_Get_Property(rapidjson source_dir)
set(rapidjson_include_dir ${source_dir}/include)
 