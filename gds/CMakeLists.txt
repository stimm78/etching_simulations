cmake_minimum_required(VERSION 3.18)
project("GDS")
add_executable(gds "gds_data.cpp")

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib64/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
target_link_libraries(gds PUBLIC OpenVDB::openvdb)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
target_link_libraries(gds PUBLIC -l:libGDSII.a)

