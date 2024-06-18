cmake_minimum_required(VERSION 3.18)
project("GDS")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include and link directories for custom libraries
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Add include directory for Triangle++
include_directories(triangle)

# Add source files
set(SOURCES
    gds_to_ply.cpp
    triangle/tpp_assert.cpp
    triangle/tpp_impl.cpp
)

# Add executable
add_executable(gds ${SOURCES})

# Link custom GDSII library
target_link_libraries(gds PUBLIC -l:libGDSII.a)

