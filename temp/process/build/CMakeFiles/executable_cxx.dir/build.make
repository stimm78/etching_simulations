# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/cmake/bin/cmake

# The command to remove a file.
RM = /opt/cmake/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fyodr/fun_CS_projects/openvdb/process

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fyodr/fun_CS_projects/openvdb/process/build

# Include any dependencies generated for this target.
include CMakeFiles/executable_cxx.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/executable_cxx.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/executable_cxx.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/executable_cxx.dir/flags.make

CMakeFiles/executable_cxx.dir/process.cxx.o: CMakeFiles/executable_cxx.dir/flags.make
CMakeFiles/executable_cxx.dir/process.cxx.o: /home/fyodr/fun_CS_projects/openvdb/process/process.cxx
CMakeFiles/executable_cxx.dir/process.cxx.o: CMakeFiles/executable_cxx.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/fyodr/fun_CS_projects/openvdb/process/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/executable_cxx.dir/process.cxx.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/executable_cxx.dir/process.cxx.o -MF CMakeFiles/executable_cxx.dir/process.cxx.o.d -o CMakeFiles/executable_cxx.dir/process.cxx.o -c /home/fyodr/fun_CS_projects/openvdb/process/process.cxx

CMakeFiles/executable_cxx.dir/process.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/executable_cxx.dir/process.cxx.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fyodr/fun_CS_projects/openvdb/process/process.cxx > CMakeFiles/executable_cxx.dir/process.cxx.i

CMakeFiles/executable_cxx.dir/process.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/executable_cxx.dir/process.cxx.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fyodr/fun_CS_projects/openvdb/process/process.cxx -o CMakeFiles/executable_cxx.dir/process.cxx.s

# Object files for target executable_cxx
executable_cxx_OBJECTS = \
"CMakeFiles/executable_cxx.dir/process.cxx.o"

# External object files for target executable_cxx
executable_cxx_EXTERNAL_OBJECTS =

executable_cxx: CMakeFiles/executable_cxx.dir/process.cxx.o
executable_cxx: CMakeFiles/executable_cxx.dir/build.make
executable_cxx: /usr/local/lib/libtbb.so
executable_cxx: /usr/local/lib/libtbbmalloc.so
executable_cxx: CMakeFiles/executable_cxx.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/fyodr/fun_CS_projects/openvdb/process/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable executable_cxx"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/executable_cxx.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/executable_cxx.dir/build: executable_cxx
.PHONY : CMakeFiles/executable_cxx.dir/build

CMakeFiles/executable_cxx.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/executable_cxx.dir/cmake_clean.cmake
.PHONY : CMakeFiles/executable_cxx.dir/clean

CMakeFiles/executable_cxx.dir/depend:
	cd /home/fyodr/fun_CS_projects/openvdb/process/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fyodr/fun_CS_projects/openvdb/process /home/fyodr/fun_CS_projects/openvdb/process /home/fyodr/fun_CS_projects/openvdb/process/build /home/fyodr/fun_CS_projects/openvdb/process/build /home/fyodr/fun_CS_projects/openvdb/process/build/CMakeFiles/executable_cxx.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/executable_cxx.dir/depend
