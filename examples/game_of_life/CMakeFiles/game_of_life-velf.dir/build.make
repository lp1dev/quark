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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lp1/Projects/quark/examples/game_of_life

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lp1/Projects/quark/examples/game_of_life

# Utility rule file for game_of_life-velf.

# Include any custom commands dependencies for this target.
include CMakeFiles/game_of_life-velf.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/game_of_life-velf.dir/progress.make

CMakeFiles/game_of_life-velf: game_of_life
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/lp1/Projects/quark/examples/game_of_life/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Converting to Sony ELF game_of_life.velf"
	/usr/local/vitasdk/bin/vita-elf-create /home/lp1/Projects/quark/examples/game_of_life/game_of_life /home/lp1/Projects/quark/examples/game_of_life/game_of_life.velf

game_of_life-velf: CMakeFiles/game_of_life-velf
game_of_life-velf: CMakeFiles/game_of_life-velf.dir/build.make
.PHONY : game_of_life-velf

# Rule to build all files generated by this target.
CMakeFiles/game_of_life-velf.dir/build: game_of_life-velf
.PHONY : CMakeFiles/game_of_life-velf.dir/build

CMakeFiles/game_of_life-velf.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/game_of_life-velf.dir/cmake_clean.cmake
.PHONY : CMakeFiles/game_of_life-velf.dir/clean

CMakeFiles/game_of_life-velf.dir/depend:
	cd /home/lp1/Projects/quark/examples/game_of_life && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lp1/Projects/quark/examples/game_of_life /home/lp1/Projects/quark/examples/game_of_life /home/lp1/Projects/quark/examples/game_of_life /home/lp1/Projects/quark/examples/game_of_life /home/lp1/Projects/quark/examples/game_of_life/CMakeFiles/game_of_life-velf.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/game_of_life-velf.dir/depend
