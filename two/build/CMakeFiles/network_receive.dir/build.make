# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\computer_network\two

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\computer_network\two\build

# Include any dependencies generated for this target.
include CMakeFiles/network_receive.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/network_receive.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/network_receive.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/network_receive.dir/flags.make

CMakeFiles/network_receive.dir/codegen:
.PHONY : CMakeFiles/network_receive.dir/codegen

CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj: CMakeFiles/network_receive.dir/flags.make
CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj: CMakeFiles/network_receive.dir/includes_CXX.rsp
CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj: D:/computer_network/two/receive/Ethernet.cpp
CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj: CMakeFiles/network_receive.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\computer_network\two\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj -MF CMakeFiles\network_receive.dir\receive\Ethernet.cpp.obj.d -o CMakeFiles\network_receive.dir\receive\Ethernet.cpp.obj -c D:\computer_network\two\receive\Ethernet.cpp

CMakeFiles/network_receive.dir/receive/Ethernet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/network_receive.dir/receive/Ethernet.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\computer_network\two\receive\Ethernet.cpp > CMakeFiles\network_receive.dir\receive\Ethernet.cpp.i

CMakeFiles/network_receive.dir/receive/Ethernet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/network_receive.dir/receive/Ethernet.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\computer_network\two\receive\Ethernet.cpp -o CMakeFiles\network_receive.dir\receive\Ethernet.cpp.s

CMakeFiles/network_receive.dir/receive/Main.cpp.obj: CMakeFiles/network_receive.dir/flags.make
CMakeFiles/network_receive.dir/receive/Main.cpp.obj: CMakeFiles/network_receive.dir/includes_CXX.rsp
CMakeFiles/network_receive.dir/receive/Main.cpp.obj: D:/computer_network/two/receive/Main.cpp
CMakeFiles/network_receive.dir/receive/Main.cpp.obj: CMakeFiles/network_receive.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\computer_network\two\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/network_receive.dir/receive/Main.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/network_receive.dir/receive/Main.cpp.obj -MF CMakeFiles\network_receive.dir\receive\Main.cpp.obj.d -o CMakeFiles\network_receive.dir\receive\Main.cpp.obj -c D:\computer_network\two\receive\Main.cpp

CMakeFiles/network_receive.dir/receive/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/network_receive.dir/receive/Main.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\computer_network\two\receive\Main.cpp > CMakeFiles\network_receive.dir\receive\Main.cpp.i

CMakeFiles/network_receive.dir/receive/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/network_receive.dir/receive/Main.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\computer_network\two\receive\Main.cpp -o CMakeFiles\network_receive.dir\receive\Main.cpp.s

CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj: CMakeFiles/network_receive.dir/flags.make
CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj: CMakeFiles/network_receive.dir/includes_CXX.rsp
CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj: D:/computer_network/two/receive/Network_ipv4_recv.cpp
CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj: CMakeFiles/network_receive.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\computer_network\two\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj -MF CMakeFiles\network_receive.dir\receive\Network_ipv4_recv.cpp.obj.d -o CMakeFiles\network_receive.dir\receive\Network_ipv4_recv.cpp.obj -c D:\computer_network\two\receive\Network_ipv4_recv.cpp

CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\computer_network\two\receive\Network_ipv4_recv.cpp > CMakeFiles\network_receive.dir\receive\Network_ipv4_recv.cpp.i

CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\computer_network\two\receive\Network_ipv4_recv.cpp -o CMakeFiles\network_receive.dir\receive\Network_ipv4_recv.cpp.s

# Object files for target network_receive
network_receive_OBJECTS = \
"CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj" \
"CMakeFiles/network_receive.dir/receive/Main.cpp.obj" \
"CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj"

# External object files for target network_receive
network_receive_EXTERNAL_OBJECTS =

network_receive.exe: CMakeFiles/network_receive.dir/receive/Ethernet.cpp.obj
network_receive.exe: CMakeFiles/network_receive.dir/receive/Main.cpp.obj
network_receive.exe: CMakeFiles/network_receive.dir/receive/Network_ipv4_recv.cpp.obj
network_receive.exe: CMakeFiles/network_receive.dir/build.make
network_receive.exe: CMakeFiles/network_receive.dir/linkLibs.rsp
network_receive.exe: CMakeFiles/network_receive.dir/objects1.rsp
network_receive.exe: CMakeFiles/network_receive.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=D:\computer_network\two\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable network_receive.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\network_receive.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/network_receive.dir/build: network_receive.exe
.PHONY : CMakeFiles/network_receive.dir/build

CMakeFiles/network_receive.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\network_receive.dir\cmake_clean.cmake
.PHONY : CMakeFiles/network_receive.dir/clean

CMakeFiles/network_receive.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\computer_network\two D:\computer_network\two D:\computer_network\two\build D:\computer_network\two\build D:\computer_network\two\build\CMakeFiles\network_receive.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/network_receive.dir/depend
