cmake_minimum_required(VERSION 3.18)
project("Test")
add_executable(Test "test.cpp")

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
target_link_libraries(Test OpenVDB::openvdb)
