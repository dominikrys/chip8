# CHIP-8 Emulator

This is a CHIP-8 emulator is written in C++17 and uses SDL2 for sound, graphics and input. It's been made with compatibility in mind due to the various CHIP-8 interpreters available.

## CHIP-8 Intro

CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s.

It has risen in popularity when the CHIP-48 interpreter was made for HP-48 graphic calculators, and hence many games are written for the CHIP-48. These rely on slightly different opcode behaviours compared to the CHIP-8 due to some of the original opcodes being undocumented, which is why this emulator includes different emulation modes to ensure compatibility.

## Screenshots

![Pong](/docs/img/pong.png)                      |![Blinky](/docs/img/blinky.png)
:-----------------------------------------------:|:----------------------------------------:
![Space Invaders](/docs/img/space_invaders.png)  |![Trip8 Demo](/docs/img/trip8_demo.png)

## Building

Dependencies: 

- **C++17 compiler**.
  - ***Windows:*** get MinGW-w64 8.0 through MSYS2 (GCC 9.2).
  - ***Linux:*** GCC 9 (can be obtained from the `ubuntu-toolchain-r/test` PPA repo).

- **SDL2**
  - ***Windows:*** download the [SDL2-2.0.10 development libraries](https://www.libsdl.org/download-2.0.php) and place them under "external" in the root directory of this project.
  - ***Linux:*** get SDL2 by running `sudo apt install cmake libsdl2-dev g++`

- **CMake 3.12**
  - ***Windows:*** can be downloaded from [here](https://cmake.org/download/).
  - ***Linux:*** can be obtained from the [Kitware apt repo](https://apt.kitware.com/).

## Usage

In terminal/command line run: `chip_8.exe --rom <path> [options]`

For more help, including displaying the available options, run: `chip_8.exe --help`

If audio is not working, set the `SDL_AUDIODRIVER` environment variable to an appropriate value mentioned [here](https://wiki.libsdl.org/FAQUsingSDL).

The CPU speed and operation modes may need to be changed between ROMs to ensure they work as intended. I've included 3 different operation modes due different ROMs relying on different opcode behaviours, depending on the time period and the interpreter they were written for. Explanations can be found in the links section. They are as follows:

- **CHIP8**: FX55 and FX65 opcodes increment the instruction counter. 8XY6 and 8XYE registers shift the value in VY and store the result in VX.

- **CHIP-48**: FX55 and FX65 opcodes increment the instruction counter.

- **SCHIP**: FX55 and FX65 opcodes don't increment the instruction counter (like on the SCHIP). This is what most ROMs expect, and is the default mode. The emulator doesn't actually support SCHIP opcodes (yet?).

## ROMs

- [Revival Studios CHIP-8 Program Pack](https://github.com/dmatlack/chip8/tree/master/roms)

- [CHIP-8 games under CC0 license](https://johnearnest.github.io/chip8Archive/)

- [BestCoder's test ROM](https://github.com/daniel5151/AC8E/blob/master/roms/bc_test.ch8)

## Links

- [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

- [Explanation of differing opcode behaviours on CHIP8/CHIP-48/SCHIP](https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/)

- [Explanation on FX55 & FX65 opcodes](https://github.com/Chromatophore/HP48-Superchip/blob/master/investigations/quirk_i.md)

- [Explanation of timer speeds](https://github.com/AfBu/haxe-CHIP-8-emulator/wiki/(Super)CHIP-8-Secrets#speed-of-emulation)
