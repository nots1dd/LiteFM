## ## ## ## ## ##
##             ##
##   LITE FM   ##
##             ##
## ## ## ## ## ## 

#/*
# * ---------------------------------------------------------------------------
# *  File:        meson.build
# *
# *  Description: Default meson.build for LiteFM
# *
# *  Author:      Siddharth Karanam
# *  Created:     22/08/24
# * 
# *  Copyright:   2024 nots1dd. All rights reserved.
# * 
# *  License:     <GNU GPL v3>
# * 
# *  Revision History:
# *      22/08/24 - Initial creation.
# *
# * ---------------------------------------------------------------------------
# */

project('litefm', 'c',
  version : '0.1',
  default_options : ['c_std=c99', 'warning_level=3'])

# Dependencies
ncurses_dep = dependency('ncursesw', required: true)
libarchive_dep = dependency('libarchive')
libyaml_dep = dependency('yaml-0.1', required : true)
sdl2_dep = dependency('sdl2')
sdl2_mixer_dep = dependency('sdl2_mixer')

# Include directories
inc_dirs = include_directories('.')

# Source files
src_files = files(
  '../lfm.c',
  '../src/cursesutils.c',
  '../src/filepreview.c',
  '../src/dircontrol.c',
  '../src/archivecontrol.c',
  '../src/clipboard.c',
  '../src/logging.c',
  '../src/signalhandling.c',
  '../src/highlight.c',
  '../src/hashtable.c',
  '../src/arg_helpers.c',
  '../src/musicpreview.c'
)

# UNCOMMENT LINES 59, 60, 68, 69 to ENABLE ASAN Memory leak VERBOSE output

# AddressSanitizer arguments
asan_c_args = ['-fsanitize=address', '-fno-omit-frame-pointer']
asan_link_args = ['-fsanitize=address']

# Executable target
executable('litefm', src_files,
  include_directories : inc_dirs,
  dependencies : [ncurses_dep, libarchive_dep, libyaml_dep, sdl2_dep, sdl2_mixer_dep],
  install : true,
  c_args : ['-Wall', '-Wextra', '-Wpedantic'],
  c_args : ['-Wall', '-Wextra', '-Wpedantic'] + asan_c_args,
  link_args : asan_link_args
)
