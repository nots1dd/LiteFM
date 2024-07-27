# Lite FM

Lightweight and Fast as FUCK file manager written in C with ncurses library

## Features

1. Ranger like traversal of directories (vim keybinds supported)
2. String search along with next occurance functionalities
3. Easy and Fast addition, deletion of files and directories
4. Clean and responsive TUI thanks to ncurses with cool colors

## Usage

-> Currently LiteFM has only been tested on the following Linux Distros:
   1. Arch Linux 6.9.9 kernels (zen and base) x86_64 arch (1920x1080)
   2. Ubuntu 22 x86_64

-> There are no plans on expanding this beyond Linux distributions

## Building

LiteFM cannot be installed in any UNIX-like distribution but can be easily built!

> DEPS
> 
> CMake / Make
> 
> Ncurses 
> 
> A C compiler (like GCC)
> 

Building with CMake:

-> `cmake -S -B build/` to create a `build/` directory with all libraries linked as per `CMakeLists.txt`

-> `cmake --build build/` to get the `./build/litefm` executable

-> Set it as an alias in your respective shell rc and enjoy!

Building with Make:

Presently, I have not made the `Makefile` yet, TBD!!

> [!TIP]
> Building LiteFM with build.sh 
> 
> chmod +x build.sh 
> 
> ./build.sh 
> 
> Thats all! Enjoy LiteFM 

## FUTURE

This file manager is far from done there are a lot of cool and essential features that are planned:

- [x] Adding a basic file preview for readable files (and stat info for directories)

- [ ] Simple tasks like selecting multiple files, moving fils/dirs to a location (cut, copy , paste)

- [ ] GO-TO a particular file or directory through string input (might be tough)

- [x] Color coding for file groups (green - general files, pink - images, red - unextracted archives, orange - audio files) [implementation through struct mostly]

- [ ] Bugfixes and massive code refactor

- [ ] Improve the build script

- [ ] Integration of adding a text editor
