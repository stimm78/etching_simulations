cmake_minimum_required(VERSION 3.18)
project("GDS")

# Add executable
add_executable(gds "gds_to_ply.cpp")

# Include and link directories for custom libraries
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
target_link_libraries(gds PUBLIC -l:libGDSII.a)
