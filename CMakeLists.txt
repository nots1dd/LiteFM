cmake_minimum_required(VERSION 3.10)

# Set the project name
project(litefm)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# Find required packages
find_package(Curses REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)

# Include directories for ncurses, libarchive, and project headers
include_directories(${CURSES_INCLUDE_DIR})
include_directories(${LIBARCHIVE_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

# Add the executable
add_executable(litefm lfm.c)

# Link required libraries
target_link_libraries(litefm ${CURSES_LIBRARIES} ${LIBARCHIVE_LIBRARIES})

# Add additional compiler flags
target_compile_options(litefm PRIVATE -Wall -Wextra -Wpedantic)
