#// // // // // // 
#//             //
#//   LITE FM   //
#//             //
#// // // // // // 

#/*
# * ---------------------------------------------------------------------------
# *  File:        CMakeLists.txt
# *
# *  Description: Default CMakeLists.txt for LiteFM
# *
# *  Author:      nots1dd
# *  Created:     31/07/24
# * 
# *  Copyright:   2024 nots1dd. All rights reserved.
# * 
# *  License:     <GNU GPL v3>
# * 
# *  Revision History:
# *      31/07/24 - Initial creation.
# *
# * ---------------------------------------------------------------------------
# */

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(litefm)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# Find required packages
find_package(Curses REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)
# Manually specify YAML include and library paths
set(YAML_INCLUDE_DIRS /usr/include)
set(YAML_LIBRARIES /usr/lib/libyaml.so)

# Include directories for ncurses, libarchive, libyaml, and project headers
include_directories(${CURSES_INCLUDE_DIR})
include_directories(${LIBARCHIVE_INCLUDE_DIRS})
include_directories(${YAML_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

# Add the executable
add_executable(litefm lfm.c src/cursesutils.c src/filepreview.c src/dircontrol.c src/archivecontrol.c src/clipboard.c src/logging.c src/signalhandling.c src/syntax.c src/AVL_tree.c)

# Link required libraries
target_link_libraries(litefm ${CURSES_LIBRARIES} ${LIBARCHIVE_LIBRARIES} ${YAML_LIBRARIES})

# Add additional compiler flags
target_compile_options(litefm PRIVATE -Wall -Wextra -Wpedantic)
