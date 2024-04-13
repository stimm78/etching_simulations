cmake_minimum_required(VERSION 3.18)
project("Cookbook")
add_executable(Cookbook "cookbook.cpp")

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
target_link_libraries(Cookbook OpenVDB::openvdb)
