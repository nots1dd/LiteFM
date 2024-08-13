# Lite FM

Lightweight and Fast as FUCK file manager written in C with ncurses library

## Features

1. Ranger like traversal of directories (vim keybinds supported)
2. String search along with next occurance functionalities
3. Easy and Fast addition, deletion of files and directories
4. Clean and responsive TUI thanks to ncurses with cool colors and unicodes
5. Incredibly fast extraction and compression functions thanks to `libarchive`
6. Very transient and logical workflow


![lfm1](https://github.com/user-attachments/assets/4bf801e2-594e-4259-b910-11a5a277de5b)
![lfm2](https://github.com/user-attachments/assets/819dd99c-69c1-456f-a92b-fdb60a7c558e)
![lfm3](https://github.com/user-attachments/assets/a3273802-4dd2-4642-a5f4-512570591a11)


> [!NOTE]
> 
> **For the best experience of LiteFM:**
> --------------------------------------
> 
> **1. Ensure that your terminal has any NERD FONT enabled.**
> 
> Without a nerd font, LiteFM will show some unreadable chars!
> 
> **2. Have it full screen whenever possible:**
> 
> Despite having dynamic window scaling, in a lot of
> the outputs the strings may look weird (or not visible at all!) upon resizing.
> Therefore, till I find a way to fix this issue, it is **HIGHLY**
> recommended to use LiteFM at: 
> 
>   -> Full screen of originally created terminal [DE] 
> 
>   -> Full terminal size without resizing [WM]
>
> **3. Launch LiteFM with `superuser`**
>
> LiteFM allows for fast file/dir actions,
> as such it is optimal to use LiteFM with any user
> that has `sudo` permissions to make every LiteFM
> incredibly productive.


## Usage

-> Currently LiteFM has only been tested on the following Linux Distros:
   1. Arch Linux kernels >= 6.9 (zen and base) x86_64 arch (1920x1080)
   2. Ubuntu versions >= 20 x86_64 arch

-> There are no plans on expanding this beyond Linux distributions

> [!WARNING]
> 
> ### Yanking
> 
> Every Yanking function only work with `WAYLAND` and `X11` display servers!!
> 
> If you are on other display servers like `Mir`, `SurfaceFlinger` and yanking does NOT work:
> 
> You can create an issue or even submit a PR as per `CONTRIBUTION.md`
> 
> ### Environment Vars
> 
> LiteFM uses `getenv` to get the env vars `$EDITOR`, `$VISUAL` and `WAYLAND_SERVER` to detect the default editor, default image viewer and display server.
> 
> Hence, ensure that you have your preferred display server and editor setup in your UNIX system so that LiteFM can be even more productive to you!
> 
> To set an env var: `echo 'export $EDITOR=/path/to/editor' >> ~/yourshellrc`
> 
> Setting your `$EDITOR` as any editor that creates a new window that resizes the main litefm window, will lead to litefm breaking so, (NOT LIKELY)
> 
> It is HIGHLY recommended that you pick a `terminal editor` like `nano`, `vi`, `vim`, `nvim`, so on
>
> You can setup `$VISUAL` as whatever you please (even if it creates a new window and resizes litefm)


## Building

LiteFM cannot be installed in any UNIX-like distribution but can be easily built!

> [!IMPORTANT]
> 
> Dependencies to install for LiteFM:
> 
> **CMake / Make**
> 
> **Ncurses library** (libncurses-dev for debian)
> 
> **libarchive** (for extraction and compression)
>
> **libyaml** (for future integration of [synhas](https://github.com/nots1dd/synhash))
>
> **rsync** (for super fast copying tool for local files [rsync](https://github.com/RsyncProject/rsync))
> 
> A C compiler (like GCC)
> 

#### Building with CMake:

**Ensure you have CMake installed first**

-> `cmake -S . -B build/` to create a `build/` directory with all libraries linked as per `CMakeLists.txt`

-> `cmake --build build/` to get the `./build/litefm` executable

-> Set it as an alias in your respective shell rc and enjoy!

> [!IMPORTANT]
> Ensure that you set up litefm.log in your ``~/.cache/litefm/log/`` directory (create if not there)
> 
> LiteFM has a very modular logging system and keeps a track of every file/dir control that goes on in a litefm instance 
> 
> Just run the command `mkdir -p $HOME/.cache/litefm/log/` and you can run litefm without a hassle!

#### Building with Make:

-> Run `make` in the current directory

-> This should give a `./litefm` executable, just run it to enjoy LiteFM!

-> To cleanup, run `make clean`

#### Adding Man Page (Optional):

You can add the man page for LiteFM in your system by the following commands:

[NOTE: You will have to keep updating this man page manually every time a new functionality is introduced.]

`sudo cp components/litefm.1 /usr/share/man/man1`

`cd /usr/share/man/man1/ && sudo gzip litefm.1`

Thats it! The man page is ready to be used via `man litefm`

> [!NOTE]
> Building LiteFM with build.sh (**HIGHLY RECOMMENDED**) 
> 
> The script contains every method of installation, logging and setup of litefm
> 
> Just run `chmod +x build.sh` 
> 
> Execute it by `./build.sh` 
> 
> Just wait for it to setup and answer a few questions, thats it!


## SECURITY

As this is a file manager that is able to perform some VERY cool and dangerous tasks like deleting any directory recursively, I definitely have tried to set up security measures to avoid any form of code vulnerability or CWE.

Other steps I plan on taking to ensure that you are always in control of the file manager are:

1. A `litefm.log` and it's functionality has been implemented (`src/logging.c`) that will log EVERY file/dir control in every litefm instance
2. Possibly set up a trash system so that accidental deletion of any file/dir can be restored [priority/low]

Check out `SECURITY.md` for the security policy that this repository follows.


## FUTURE

This file manager is far from done there are a lot of cool and essential features that are planned:

- [x] Adding a basic file preview for readable files (and stat info for directories)

- [ ] Integrating current file tasks to multiple files selection (copy, paste, move)

- [x] GO-TO a particular file or directory through string input (might be tough)

- [x] Color coding for file groups (green - general files, pink - images, red - unextracted archives, orange - audio files) [implementation through struct mostly]

- [ ] Bugfixes and massive code refactor

- [x] Improve the build script

- [x] Integration of adding a text editor

- [x] Handling resizing of litefm window (maybe through `SIGWINCH`)

- [ ] **UPCOMING: Modular Syntax highlighting through `yaml.h` and `Hashtables`**

- [ ] **UPCOMING: Adding a privacy guard (using GPG maybe)**
