# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build"

# Include any dependencies generated for this target.
include CMakeFiles/net_client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/net_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/net_client.dir/flags.make

CMakeFiles/net_client.dir/src/SimpleClient.cpp.o: CMakeFiles/net_client.dir/flags.make
CMakeFiles/net_client.dir/src/SimpleClient.cpp.o: ../src/SimpleClient.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/net_client.dir/src/SimpleClient.cpp.o"
	clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/net_client.dir/src/SimpleClient.cpp.o -c "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/src/SimpleClient.cpp"

CMakeFiles/net_client.dir/src/SimpleClient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net_client.dir/src/SimpleClient.cpp.i"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/src/SimpleClient.cpp" > CMakeFiles/net_client.dir/src/SimpleClient.cpp.i

CMakeFiles/net_client.dir/src/SimpleClient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net_client.dir/src/SimpleClient.cpp.s"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/src/SimpleClient.cpp" -o CMakeFiles/net_client.dir/src/SimpleClient.cpp.s

# Object files for target net_client
net_client_OBJECTS = \
"CMakeFiles/net_client.dir/src/SimpleClient.cpp.o"

# External object files for target net_client
net_client_EXTERNAL_OBJECTS =

net_client: CMakeFiles/net_client.dir/src/SimpleClient.cpp.o
net_client: CMakeFiles/net_client.dir/build.make
net_client: CMakeFiles/net_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable net_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/net_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/net_client.dir/build: net_client

.PHONY : CMakeFiles/net_client.dir/build

CMakeFiles/net_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/net_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/net_client.dir/clean

CMakeFiles/net_client.dir/depend:
	cd "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2" "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2" "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build" "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build" "/home/yjh/Nutstore Files/Code_for_linux/C++Primer/MMO_ASIO/NetCommons/Part1_2/build/CMakeFiles/net_client.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/net_client.dir/depend

