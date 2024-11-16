cmake_minimum_required(VERSION 3.10)
project(YAC_SIM)

# Set the C++ standard and compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pedantic")

include(CheckCXXSourceCompiles)

# Check for __builtin_ctz
check_cxx_source_compiles("
    int main() {
        int x = __builtin_ctz(1);
        return 0;
    }" HAVE_BUILTIN_CTZ)

# Check for __builtin_popcount
check_cxx_source_compiles("
    int main() {
        int x = __builtin_popcount(1);
        return 0;
    }" HAVE_BUILTIN_POPCOUNT)

# Add compile definitions based on the checks
if (HAVE_BUILTIN_CTZ)
    add_compile_definitions(HAVE_BUILTIN_CTZ)
endif()

if (HAVE_BUILTIN_POPCOUNT)
    add_compile_definitions(HAVE_BUILTIN_POPCOUNT)
endif()

# Add the src directory as an include path for all targets
include_directories(${CMAKE_SOURCE_DIR}/src)

# Enable testing with CTest
enable_testing()

# Add the tests subdirectory
add_subdirectory(tests)

# Add source files
add_library(utilities src/utilities.cpp "src/main.cpp" "src/yacsim.cpp")

# Set CMake to export compile commands in the build directory
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If the build directory is separate, create a symbolic link in the root
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    # Nothing to do if the build directory is the source directory
else()
    # Create a symbolic link to compile_commands.json in the root directory
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                    ${CMAKE_BINARY_DIR}/compile_commands.json
                    ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()

# Add the executable target
#add_executable(yac_sim yac_sim.cpp utils.cpp)

# Specify the directories to clean with 'make clean'
#set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "yac_sim;*.o;GPATH;GTAGS;GRTAGS")
