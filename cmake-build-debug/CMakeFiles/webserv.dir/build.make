# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_COMMAND = /app/extra/clion/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /app/extra/clion/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sandre-a/42_cursus/webserv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sandre-a/42_cursus/webserv/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/webserv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/webserv.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/webserv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/webserv.dir/flags.make

CMakeFiles/webserv.dir/webserv/main.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/webserv/main.cpp.o: /home/sandre-a/42_cursus/webserv/webserv/main.cpp
CMakeFiles/webserv.dir/webserv/main.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/webserv.dir/webserv/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/webserv/main.cpp.o -MF CMakeFiles/webserv.dir/webserv/main.cpp.o.d -o CMakeFiles/webserv.dir/webserv/main.cpp.o -c /home/sandre-a/42_cursus/webserv/webserv/main.cpp

CMakeFiles/webserv.dir/webserv/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/webserv.dir/webserv/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sandre-a/42_cursus/webserv/webserv/main.cpp > CMakeFiles/webserv.dir/webserv/main.cpp.i

CMakeFiles/webserv.dir/webserv/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/webserv/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sandre-a/42_cursus/webserv/webserv/main.cpp -o CMakeFiles/webserv.dir/webserv/main.cpp.s

CMakeFiles/webserv.dir/webserv/Config.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/webserv/Config.cpp.o: /home/sandre-a/42_cursus/webserv/webserv/Config.cpp
CMakeFiles/webserv.dir/webserv/Config.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/webserv.dir/webserv/Config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/webserv/Config.cpp.o -MF CMakeFiles/webserv.dir/webserv/Config.cpp.o.d -o CMakeFiles/webserv.dir/webserv/Config.cpp.o -c /home/sandre-a/42_cursus/webserv/webserv/Config.cpp

CMakeFiles/webserv.dir/webserv/Config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/webserv.dir/webserv/Config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sandre-a/42_cursus/webserv/webserv/Config.cpp > CMakeFiles/webserv.dir/webserv/Config.cpp.i

CMakeFiles/webserv.dir/webserv/Config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/webserv/Config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sandre-a/42_cursus/webserv/webserv/Config.cpp -o CMakeFiles/webserv.dir/webserv/Config.cpp.s

CMakeFiles/webserv.dir/webserv/Server.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/webserv/Server.cpp.o: /home/sandre-a/42_cursus/webserv/webserv/Server.cpp
CMakeFiles/webserv.dir/webserv/Server.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/webserv.dir/webserv/Server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/webserv/Server.cpp.o -MF CMakeFiles/webserv.dir/webserv/Server.cpp.o.d -o CMakeFiles/webserv.dir/webserv/Server.cpp.o -c /home/sandre-a/42_cursus/webserv/webserv/Server.cpp

CMakeFiles/webserv.dir/webserv/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/webserv.dir/webserv/Server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sandre-a/42_cursus/webserv/webserv/Server.cpp > CMakeFiles/webserv.dir/webserv/Server.cpp.i

CMakeFiles/webserv.dir/webserv/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/webserv/Server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sandre-a/42_cursus/webserv/webserv/Server.cpp -o CMakeFiles/webserv.dir/webserv/Server.cpp.s

CMakeFiles/webserv.dir/webserv/utils.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/webserv/utils.cpp.o: /home/sandre-a/42_cursus/webserv/webserv/utils.cpp
CMakeFiles/webserv.dir/webserv/utils.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/webserv.dir/webserv/utils.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/webserv/utils.cpp.o -MF CMakeFiles/webserv.dir/webserv/utils.cpp.o.d -o CMakeFiles/webserv.dir/webserv/utils.cpp.o -c /home/sandre-a/42_cursus/webserv/webserv/utils.cpp

CMakeFiles/webserv.dir/webserv/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/webserv.dir/webserv/utils.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sandre-a/42_cursus/webserv/webserv/utils.cpp > CMakeFiles/webserv.dir/webserv/utils.cpp.i

CMakeFiles/webserv.dir/webserv/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/webserv/utils.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sandre-a/42_cursus/webserv/webserv/utils.cpp -o CMakeFiles/webserv.dir/webserv/utils.cpp.s

# Object files for target webserv
webserv_OBJECTS = \
"CMakeFiles/webserv.dir/webserv/main.cpp.o" \
"CMakeFiles/webserv.dir/webserv/Config.cpp.o" \
"CMakeFiles/webserv.dir/webserv/Server.cpp.o" \
"CMakeFiles/webserv.dir/webserv/utils.cpp.o"

# External object files for target webserv
webserv_EXTERNAL_OBJECTS =

webserv: CMakeFiles/webserv.dir/webserv/main.cpp.o
webserv: CMakeFiles/webserv.dir/webserv/Config.cpp.o
webserv: CMakeFiles/webserv.dir/webserv/Server.cpp.o
webserv: CMakeFiles/webserv.dir/webserv/utils.cpp.o
webserv: CMakeFiles/webserv.dir/build.make
webserv: CMakeFiles/webserv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable webserv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/webserv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/webserv.dir/build: webserv
.PHONY : CMakeFiles/webserv.dir/build

CMakeFiles/webserv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/webserv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/webserv.dir/clean

CMakeFiles/webserv.dir/depend:
	cd /home/sandre-a/42_cursus/webserv/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sandre-a/42_cursus/webserv /home/sandre-a/42_cursus/webserv /home/sandre-a/42_cursus/webserv/cmake-build-debug /home/sandre-a/42_cursus/webserv/cmake-build-debug /home/sandre-a/42_cursus/webserv/cmake-build-debug/CMakeFiles/webserv.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/webserv.dir/depend

