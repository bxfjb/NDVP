# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zyh/ndvp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zyh/ndvp/build

# Include any dependencies generated for this target.
include CMakeFiles/NetworkSystem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/NetworkSystem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/NetworkSystem.dir/flags.make

CMakeFiles/NetworkSystem.dir/main.cpp.o: CMakeFiles/NetworkSystem.dir/flags.make
CMakeFiles/NetworkSystem.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zyh/ndvp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/NetworkSystem.dir/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/NetworkSystem.dir/main.cpp.o -c /home/zyh/ndvp/main.cpp

CMakeFiles/NetworkSystem.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/NetworkSystem.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zyh/ndvp/main.cpp > CMakeFiles/NetworkSystem.dir/main.cpp.i

CMakeFiles/NetworkSystem.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/NetworkSystem.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zyh/ndvp/main.cpp -o CMakeFiles/NetworkSystem.dir/main.cpp.s

CMakeFiles/NetworkSystem.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/NetworkSystem.dir/main.cpp.o.requires

CMakeFiles/NetworkSystem.dir/main.cpp.o.provides: CMakeFiles/NetworkSystem.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/NetworkSystem.dir/build.make CMakeFiles/NetworkSystem.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/NetworkSystem.dir/main.cpp.o.provides

CMakeFiles/NetworkSystem.dir/main.cpp.o.provides.build: CMakeFiles/NetworkSystem.dir/main.cpp.o


CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o: CMakeFiles/NetworkSystem.dir/flags.make
CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o: ../ndvp/ndvp.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zyh/ndvp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o -c /home/zyh/ndvp/ndvp/ndvp.cpp

CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zyh/ndvp/ndvp/ndvp.cpp > CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.i

CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zyh/ndvp/ndvp/ndvp.cpp -o CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.s

CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.requires:

.PHONY : CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.requires

CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.provides: CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.requires
	$(MAKE) -f CMakeFiles/NetworkSystem.dir/build.make CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.provides.build
.PHONY : CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.provides

CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.provides.build: CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o


CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o: CMakeFiles/NetworkSystem.dir/flags.make
CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o: ../ndvp/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zyh/ndvp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o -c /home/zyh/ndvp/ndvp/util.cpp

CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zyh/ndvp/ndvp/util.cpp > CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.i

CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zyh/ndvp/ndvp/util.cpp -o CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.s

CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.requires:

.PHONY : CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.requires

CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.provides: CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.requires
	$(MAKE) -f CMakeFiles/NetworkSystem.dir/build.make CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.provides.build
.PHONY : CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.provides

CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.provides.build: CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o


# Object files for target NetworkSystem
NetworkSystem_OBJECTS = \
"CMakeFiles/NetworkSystem.dir/main.cpp.o" \
"CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o" \
"CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o"

# External object files for target NetworkSystem
NetworkSystem_EXTERNAL_OBJECTS =

NetworkSystem: CMakeFiles/NetworkSystem.dir/main.cpp.o
NetworkSystem: CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o
NetworkSystem: CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o
NetworkSystem: CMakeFiles/NetworkSystem.dir/build.make
NetworkSystem: CMakeFiles/NetworkSystem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zyh/ndvp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable NetworkSystem"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/NetworkSystem.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/NetworkSystem.dir/build: NetworkSystem

.PHONY : CMakeFiles/NetworkSystem.dir/build

CMakeFiles/NetworkSystem.dir/requires: CMakeFiles/NetworkSystem.dir/main.cpp.o.requires
CMakeFiles/NetworkSystem.dir/requires: CMakeFiles/NetworkSystem.dir/ndvp/ndvp.cpp.o.requires
CMakeFiles/NetworkSystem.dir/requires: CMakeFiles/NetworkSystem.dir/ndvp/util.cpp.o.requires

.PHONY : CMakeFiles/NetworkSystem.dir/requires

CMakeFiles/NetworkSystem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NetworkSystem.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NetworkSystem.dir/clean

CMakeFiles/NetworkSystem.dir/depend:
	cd /home/zyh/ndvp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zyh/ndvp /home/zyh/ndvp /home/zyh/ndvp/build /home/zyh/ndvp/build /home/zyh/ndvp/build/CMakeFiles/NetworkSystem.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NetworkSystem.dir/depend
