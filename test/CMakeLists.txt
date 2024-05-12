cmake_minimum_required(VERSION 3.18)
project("Test")
add_executable(Test "test.cpp")

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib64/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
# find_package(Boost REQUIRED)
target_link_libraries(Test PUBLIC OpenVDB::openvdb)
# target_link_libraries(Test PUBLIC Boost::headers)
# target_include_directories(Test SYSTEM PUBLIC ${Boost_INCLUDE_DIR})

