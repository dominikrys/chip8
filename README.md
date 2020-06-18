# CHIP-8 Emulator

[![Build Status](https://travis-ci.com/dominikrys/chip-8-emulator.svg?branch=master)](https://travis-ci.com/dominikrys/chip-8-emulator)
[![Website](https://img.shields.io/website?down_color=lightgrey&down_message=offline&up_color=brightgreen&up_message=online&url=https%3A%2F%2Fdominikrys.com%2Fchip-8%2F)](https://dominikrys.com/chip-8/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/release/dominikrys/chip-8-emulator.svg)](https://github.com/dominikrys/chip-8-emulator/releases)

This is a CHIP-8 emulator written in C++17 which uses SDL2 for sound, graphics and input.

[**Compiled to WebAssembly and hosted on the web**](http://dominikrys.com/chip-8).

Compiled Linux binaries available under [releases](https://github.com/dominikrys/chip-8-emulator/releases).

_CHIP-8 is an interpreted programming language developed by Joseph Weisbecker in the 1970s. It was made to allow video games to be more easily programmed for 8-bit microcomputers at the time, and runs on a CHIP-8 virtual machine._

## Screenshots

|           ![Pong](docs/img/pong.png)           |     ![Blinky](docs/img/blinky.png)     |
| :--------------------------------------------: | :------------------------------------: |
| ![Space Invaders](docs/img/space_invaders.png) | ![Trip8 Demo](docs/img/trip8_demo.png) |

## Building

### Dependencies

- **C++17 compiler**

  - **Linux:** GCC 9. Can be obtained by installing `g++-9` from the `ubuntu-toolchain-r/test` PPA repo.
  - **Windows:** MinGW-w64 8.0 (GCC 9.2). Can be obtained through MSYS2 by installing the `mingw-w64-x86_64-gcc` pacman package after updating MSYS2 with `pacman -Syu`. To run, download the [SDL2 runtime binaries](https://www.libsdl.org/download-2.0.php) and put SDL2.dll into the folder with your compiled binary

- **SDL2**

  - **Linux:** get SDL2 by running `sudo apt install libsdl2-dev`.
  - **Windows:** download the [SDL2-2.0.10 development libraries](https://www.libsdl.org/download-2.0.php) and place them under a new "external" folder in the root directory of this project.

- **CMake 3.10**

  - **Linux:** can be obtained from the `ubuntu-toolchain-r/test` PPA repo or from the [Kitware apt repo](https://apt.kitware.com/).
  - **Windows:** can be downloaded from [here](https://cmake.org/download/).

- **Emscripten 1.39.17 _(For Webassembly only)_**
  - Download [here](https://emscripten.org/docs/getting_started/downloads.html).

### Compiling

Run the following from the source directory:

- **Linux:**

  `cmake . -B <output dir> -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles"`

  `cmake --build <output dir>`

- **Windows:**

  `cmake.exe . -B <output dir> -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - MinGW Makefiles"`

  `cmake.exe --build <output dir>`

- **WebAssembly:**

  - Install Emscripten: `./emsdk install latest`
  - Activate Emscripten: `./emsdk activate latest`
  - **(On Windows):** emsdk install mingw32-make using `emsdk install mingw-7.1.0-64bit`
  - Navigate to a sub-directory to where CMake files will be generated to (e.g. `chip-8-emulator/cmake-build-emscripten`)
  - **(On Windows):** Run `emcmake cmake -G "CodeBlocks - MinGW Makefiles" .. -DCMAKE_SH="CMAKE_SH-NOTFOUND"`
  - **(On Linux):** Run `emcmake cmake -G "CodeBlocks - Unix Makefiles" ..`
  - _Optionally in the above step you can specify the build type by adding `-DCMAKE_BUILD_TYPE=Debug|Release`_
  - The files have been output to `chip-8-emulator/web` directory. To run, host the `web` directory using e.g. `python3 -m http.server` and access `http://localhost:8000/` locally.

## Usage

**Linux**: `./chip_8 --rom <path> [options]`

**Windows**: `chip_8.exe --rom <path> [options]`

- For more help, including displaying the available options, run: `chip_8.exe --help`

- Some ROMs are provided in the /bin/roms directory.

- If audio is not working, set the `SDL_AUDIODRIVER` environment variable to an appropriate value mentioned [here](https://wiki.libsdl.org/FAQUsingSDL).

- The CPU speed and operation modes may need to be changed between ROMs to ensure they work as intended. I've included 3 different operation modes due different ROMs relying on different opcode behaviours, depending on the time period and the interpreter they were written for. Explanations can be found in the links section. They are as follows:

  - **CHIP8**: FX55 and FX65 opcodes increment the instruction counter. 8XY6 and 8XYE registers shift the value in VY and store the result in VX.

  - **CHIP-48**: FX55 and FX65 opcodes increment the instruction counter.

  - **SCHIP**: FX55 and FX65 opcodes don't increment the instruction counter (like on the SCHIP). This is what most ROMs expect, and is the default mode. The emulator doesn't actually support SCHIP opcodes (yet?).

## Links

- [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

- [Explanation of differing opcode behaviours on CHIP8/CHIP-48/SCHIP](https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/)

- [Explanation on FX55 & FX65 opcodes](https://github.com/Chromatophore/HP48-Superchip/blob/master/investigations/quirk_i.md)

- [Explanation of timer speeds](<https://github.com/AfBu/haxe-CHIP-8-emulator/wiki/(Super)CHIP-8-Secrets#speed-of-emulation>)
