## ## ## ## ## ##
##             ##
##   LITE FM   ##
##             ##
## ## ## ## ## ##


#/*
# * ---------------------------------------------------------------------------
# *  File:        CMakeLists.txt
# *
# *  Description: Default CMakeLists.txt for LiteFM
# *
# *  Author:      Siddharth Karanam
# *  Created:     31/07/24
# * 
# *  Copyright:   2024 nots1dd. All rights reserved.
# * 
# *  License:     <GNU GPL v3>
# * 
# *  Revision History:
# *      31/07/24 - Initial creation.
# *      22/08/24 - Added ASan + other deps
# *
# * ---------------------------------------------------------------------------
# */

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(litefm)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# ---------------------------------------------------------------------------------------
#                                    IMPORTANT
#
# Ensure that you modify this CMakeLists.txt iff:
#
# 1. You are on a Debian distribution and UNICODE does NOT show up properly
#    -> You will need the `ncursesw5` package with wide character support
# 2. If some package has a different name (OR) pkg-config is NOT able to find it 
# 3. Obviously modify this at your own discretion and do what it takes to make it work.
# 
# ---------------------------------------------------------------------------------------

# Find required packages
find_package(Curses REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)
pkg_check_modules(LIBYAML REQUIRED yaml-0.1)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# Include directories for ncurses, libarchive, libyaml, SDL2, SDL2_mixer, and project headers
include_directories(${CURSES_INCLUDE_DIR})
include_directories(${LIBARCHIVE_INCLUDE_DIRS})
include_directories(${LIBYAML_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

# Add the executable
add_executable(litefm lfm.c src/cursesutils.c src/filepreview.c src/dircontrol.c src/archivecontrol.c src/clipboard.c src/logging.c src/highlight.c src/hashtable.c src/arg_helpers.c src/musicpreview.c src/inodeinfo.c src/kbinput.c)

# Link required libraries
target_link_libraries(litefm ${CURSES_LIBRARIES} ${LIBARCHIVE_LIBRARIES} ${LIBYAML_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES})

# Add additional compiler flags
target_compile_options(litefm PRIVATE -Wall -Wextra -Wpedantic)
