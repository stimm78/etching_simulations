cmake_minimum_required(VERSION 3.10)

set(CXX_TARGET_FILENAME cook_makesphere.cxx)

# set the project name and version
project(executable_cxx)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(TBB_ROOT "path/to/tbb")
# """Replace with the actual path to your TBB installation directory"""
# Apparently some sources expect tbb to have its own "include" and "lib" directories??
# Whatever, it works now!
set(TBB_INCLUDE_DIR "/usr/local/include/tbb")
include_directories(${TBB_INCLUDE_DIRS})

set(TBB_LIBRARY_DIR "/usr/local/lib")
find_library(TBB_LIBRARY NAMES tbb HINTS ${TBB_LIBRARY_DIR} REQUIRED)
find_library(TBB_MALLOC_LIBRARY NAMES tbbmalloc HINTS ${TBB_LIBRARY_DIR} REQUIRED)
if (TBB_LIBRARY AND TBB_MALLOC_LIBRARY)
    set(TBB_LIBRARIES ${TBB_LIBRARY} ${TBB_MALLOC_LIBRARY})
else ()
    message(FATAL_ERROR "TBB libraries not found. Please specify the correct path to TBB libraries.")
endif ()

#find_library()

#message(STATUS "MESSAGE: ${TBB_LIBRARIES}")

add_executable(executable_cxx ${CXX_TARGET_FILENAME})

# Use target_link_libraries to link libraries to our executable
target_link_libraries(executable_cxx libopenvdb.so.11.0.1)
target_link_libraries(executable_cxx ${TBB_LIBRARIES})

# Hint: ${PROJECT_SOURCE_DIR} is a path to the project source. AKA This folder!

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
# target_include_directories(Tutorial PUBLIC
#                             "${PROJECT_BINARY_DIR}"
#                             "${PROJECT_SOURCE_DIR}/MathFunctions"
#                            )
