# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /home/aunitily/clion/clion-2021.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/aunitily/clion/clion-2021.2.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aunitily/CPTS-360-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aunitily/CPTS-360-master/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CPTS_360_master.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/CPTS_360_master.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CPTS_360_master.dir/flags.make

CMakeFiles/CPTS_360_master.dir/main.c.o: CMakeFiles/CPTS_360_master.dir/flags.make
CMakeFiles/CPTS_360_master.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aunitily/CPTS-360-master/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CPTS_360_master.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CPTS_360_master.dir/main.c.o -c /home/aunitily/CPTS-360-master/main.c

CMakeFiles/CPTS_360_master.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPTS_360_master.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aunitily/CPTS-360-master/main.c > CMakeFiles/CPTS_360_master.dir/main.c.i

CMakeFiles/CPTS_360_master.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPTS_360_master.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aunitily/CPTS-360-master/main.c -o CMakeFiles/CPTS_360_master.dir/main.c.s

# Object files for target CPTS_360_master
CPTS_360_master_OBJECTS = \
"CMakeFiles/CPTS_360_master.dir/main.c.o"

# External object files for target CPTS_360_master
CPTS_360_master_EXTERNAL_OBJECTS =

CPTS_360_master: CMakeFiles/CPTS_360_master.dir/main.c.o
CPTS_360_master: CMakeFiles/CPTS_360_master.dir/build.make
CPTS_360_master: CMakeFiles/CPTS_360_master.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aunitily/CPTS-360-master/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable CPTS_360_master"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CPTS_360_master.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CPTS_360_master.dir/build: CPTS_360_master
.PHONY : CMakeFiles/CPTS_360_master.dir/build

CMakeFiles/CPTS_360_master.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CPTS_360_master.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CPTS_360_master.dir/clean

CMakeFiles/CPTS_360_master.dir/depend:
	cd /home/aunitily/CPTS-360-master/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aunitily/CPTS-360-master /home/aunitily/CPTS-360-master /home/aunitily/CPTS-360-master/cmake-build-debug /home/aunitily/CPTS-360-master/cmake-build-debug /home/aunitily/CPTS-360-master/cmake-build-debug/CMakeFiles/CPTS_360_master.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CPTS_360_master.dir/depend

