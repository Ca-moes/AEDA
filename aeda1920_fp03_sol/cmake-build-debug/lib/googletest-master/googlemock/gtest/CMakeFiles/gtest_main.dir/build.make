# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.2.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.2.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug"

# Include any dependencies generated for this target.
include lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/depend.make

# Include the progress variables for this target.
include lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/progress.make

# Include the compile flags for this target's objects.
include lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make

lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj: lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make
lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj: lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/includes_CXX.rsp
lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj: ../lib/googletest-master/googletest/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj"
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\gtest_main.dir\src\gtest_main.cc.obj -c "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\lib\googletest-master\googletest\src\gtest_main.cc"

lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest_main.dir/src/gtest_main.cc.i"
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\lib\googletest-master\googletest\src\gtest_main.cc" > CMakeFiles\gtest_main.dir\src\gtest_main.cc.i

lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest_main.dir/src/gtest_main.cc.s"
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\lib\googletest-master\googletest\src\gtest_main.cc" -o CMakeFiles\gtest_main.dir\src\gtest_main.cc.s

# Object files for target gtest_main
gtest_main_OBJECTS = \
"CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj"

# External object files for target gtest_main
gtest_main_EXTERNAL_OBJECTS =

lib/libgtest_maind.a: lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.obj
lib/libgtest_maind.a: lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/build.make
lib/libgtest_maind.a: lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ..\..\..\libgtest_maind.a"
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && $(CMAKE_COMMAND) -P CMakeFiles\gtest_main.dir\cmake_clean_target.cmake
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\gtest_main.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/build: lib/libgtest_maind.a

.PHONY : lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/build

lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/clean:
	cd /d C:\Data\Andre\Work\MIEIC-~3\AEDA\AE8747~1\CMAKE-~1\lib\GOOGLE~1\GOOGLE~1\gtest && $(CMAKE_COMMAND) -P CMakeFiles\gtest_main.dir\cmake_clean.cmake
.PHONY : lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/clean

lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol" "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\lib\googletest-master\googletest" "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug" "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug\lib\googletest-master\googlemock\gtest" "C:\Data\Andre\Work\MIEIC - ANO 2 - SEMESTRE 1\AEDA\aeda1920_fp03_sol\cmake-build-debug\lib\googletest-master\googlemock\gtest\CMakeFiles\gtest_main.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : lib/googletest-master/googlemock/gtest/CMakeFiles/gtest_main.dir/depend

