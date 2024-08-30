# Lite FM

Light and fast file manager meant to be useful to power users 

Written in C with NCurses, libyaml, rsync and more!

## Features

1. Ranger like traversal of directories (vim keybinds supported)
2. String search along with next occurance functionalities
3. Easy and Fast addition, deletion of files and directories
4. Clean and responsive TUI thanks to ncurses with cool colors and unicodes
5. Incredibly fast extraction and compression functions thanks to `libarchive`
6. Very transient and logical workflow
7. Insanely **FAST** and **MODULAR** Syntax Highlighting system [SYNHASH](https://github.com/nots1dd/synhash)


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
> **2. Dynamic resizing:**
> 
> There is dynamic windowing setup in LiteFM. However,
> 
> Upon resizing, you will have to press any key for it to show!
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
   3. Linux Mint >= 20 x86_64 arch

-> There are no plans on expanding this beyond Linux distributions

> [!WARNING]
> 
> ### Yanking
> 
> The yank function `y (getting file location)` only work with `WAYLAND` and `X11` display servers!!
> 
> If you are on other display servers like `Mir`, `SurfaceFlinger` and yanking does NOT work:
> 
> You can create an issue or even submit a PR as per **[CONTRIBUTION.md](https://github.com/nots1dd/blob/main/CONTRIBUTION.md)**
> 
> ### Environment Vars
> 
> LiteFM uses `getenv` to get the env vars `$EDITOR`, `$VISUAL` and `WAYLAND_SERVER` to detect the default editor, default image viewer and display server.
> 
> Hence, ensure that you have your preferred display server and editor setup in your UNIX system so that LiteFM can be even more productive to you!
> 
> To set an env var: 
```sh 
echo 'export EDITOR=/sbin/nvim' >> ~/.bashrc
```
> You can set your desired **$EDITOR** and shell rc.
> 
> Setting your **$EDITOR** as any editor that creates a new window that resizes the main litefm window, will lead to litefm to look a bit weird (it will resize)
> 
> It is HIGHLY recommended that you pick a **terminal editor** like `nano`, `vi`, `vim`, `nvim`, so on
> 
> Also for **$VISUAL** it is recommended that you go for something minimal like `swappy`, `satty`, so on
> 
> You can setup **$VISUAL** as whatever you please (even if it creates a new window and resizes litefm)

## Dependencies

As LiteFM can only be built manually, here is a table of all deps used:


| Package Manager | Debian                    | Fedora (RPM)             | Arch Linux              |
|-----------------|---------------------------|--------------------------|-------------------------|
| **64-bit**      |                           |                          |                         |
|                 | libncursesw5-dev          | ncurses-devel            | ncurses                 |
|                 | cmake                     | cmake                    | cmake                   |
|                 | make                      | make                     | make                    |
|                 | libarchive-dev            | libarchive-devel         | libarchive              |
|                 | libyaml-dev               | libyaml-devel            | yaml-cpp                |
|                 | rsync                     | rsync                    | rsync                   |
|                 | pkg-config                | pkg-config               | pkg-config              |
| **32-bit**      |                           |                          |                         |
|                 | libncurses5-dev           | ncurses-libs             | lib32-ncurses5          |
|                 | libarchive-dev:i386       | libarchive-devel.i686    | lib32-libarchive        |
|                 | libyaml-dev:i386          | libyaml-devel.i686       | lib32-yaml-cpp          |
|                 | rsync:i386                | rsync.i686               | lib32-rsync             |
|                 | pkg-config:i386           | pkg-config.i686          | lib32-pkg-config        |


The above are the listed **32-bit** and **64-bit** libraries that are used in this project.

->Only 64-bit libs have been tested to fruition so far 

Refer to this table to understand the **[Building](https://github.com/nots1dd/LiteFM?tab=readme-ov-file#building)** procedure better

## Building

REFER TO **[Dependencies](https://github.com/nots1dd/LiteFM?tab=readme-ov-file#dependencies)** TO BETTER UNDERSTAND THE BUILDING PROCESS AND FLOW.

LiteFM cannot be installed in any UNIX-like distribution but can be easily built!

> [!NOTE]
> Building LiteFM with build.sh (**HIGHLY RECOMMENDED**) 
> 
> The script contains every method of installation, logging and setup of litefm
> 
> Just run: 
> ```sh 
> chmod +x build.sh
> ./build.sh 
> ```
> 
> Just wait for it to setup and answer a few questions, thats it!

#### Building with CMake:

**Ensure you have CMake installed first**

-> `cmake -S . -B build/` to create a `build/` directory with all libraries linked as per `CMakeLists.txt`

-> `cmake --build build/` to get the `./build/litefm` executable

-> Set it as an alias in your respective shell rc and enjoy!

```sh
cmake -S . -B build 
cmake --build build/
./build/litefm
```

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

```sh
make 
./litefm
```

#### Building with Meson:

-> Ensure that you have `meson` and `ninja` (or `ninja-build`) installed in your distribution.

```sh
meson setup build/
meson compile -C build/
./build/litefm
```

NOTE: Running `sudo ninja install` will MOVE the litefm executable to `/usr/local/bin` directory. 

If you DO NOT want this, ignore the last step and simple run `./litefm`

In any case, to uninstall (stay in the build dir):

```sh
sudo ninja uninstall
```

-> Run the executable `litefm` and enjoy!


#### Debugging

In order check how LiteFM works under the hood and further understand any issues, redundancies or vulnerabilities,

Check out [debug](https://github.com/nots1dd/litefm/blob/main/debug) to build LiteFM with **AddressSanitizer** (ASan)


#### Adding Man Page (Optional):

You can add the man page for LiteFM in your system by the following commands:

[NOTE: You will have to keep updating this man page manually every time a new functionality is introduced.]

```sh 
sudo cp components/litefm.1 /usr/share/man/man1
cd /usr/share/man/man1/ && sudo gzip litefm.1
```

Thats it! The man page is ready to be used via `man litefm`


## SECURITY

As this is a file manager that is able to perform some VERY cool and dangerous tasks like deleting any directory recursively, I definitely have tried to set up security measures to avoid any form of code vulnerability or CWE.

Other steps I plan on taking to ensure that you are always in control of the file manager are:

1. A `litefm.log` and it's functionality has been implemented (`src/logging.c`) that will log EVERY file/dir control in every litefm instance
2. Possibly set up a trash system so that accidental deletion of any file/dir can be restored [priority/low]
3. Trying to add password protected files [priority/med]

Check out [SECURITY](https://github.com/nots1dd/litefm/blob/main/SECURITY.md) for the security policy that this repository follows.

Security Tools that can be used in LiteFM:

-> AddressSanitizer (ASan): Gives an in-depth view of how memory leaks take place, overflows and much more 

> [!NOTE]
> ASan is **DISABLED** by default
> 
> To enable it you will to undergo a **debug build**
> 
> Check out [debug build](https://github.com/nots1dd/LiteFM?tab=readme-ov-file#debugging)

## FUTURE

This file manager is far from done there are a lot of cool and essential features that are planned:

- [x] Adding a basic file preview for readable files (and stat info for directories)

- [ ] Integrating current file tasks to multiple files selection (copy, paste, move)

- [x] GO-TO a particular file or directory through string input (might be tough)

- [x] Color coding for file groups (green - general files, pink - images, red - unextracted archives, orange - audio files) [implementation through struct mostly]

- [ ] Bugfixes and massive code refactor

- [x] Improve the build script

- [x] Integration of adding a text editor

- [ ] Better handling of `dynamically linked files` and `symbolic links`

- [x] Handling resizing of litefm window (maybe through `SIGWINCH`)

- [x] **UPCOMING: Modular Syntax highlighting through `yaml.h` and `Hashtables`**

- [ ] **UPCOMING: Adding a privacy guard (using GPG maybe)**
