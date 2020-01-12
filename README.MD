# CHIP-8 Emulator

This is a CHIP-8 emulator is written in C++17 and uses SDL2 for sound, graphics and input. It's been made with compatibility in mind due to the various CHIP-8 implementations available.

## CHIP-8 Intro

CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s.

It has risen in popularity when the CHIP-48 interpreter was made for HP-48 graphic calculators, and hence many games are written for the CHIP-48. These rely on slightly different opcode behaviours compared to the CHIP-8 due to some of the original opcodes being undocumented, which is why this emulator includes different emulation modes to ensure compatibility.

## Screenshots from the emulator

![Pong](/docs/img/pong.png)                      |![Blinky](/docs/img/blinky.png)
:-----------------------------------------------:|:----------------------------------------:
![Space Invaders](/docs/img/space_invaders.png)  |![Trip8 Demo](/docs/img/trip8_demo.png)

## Building

Dependencies: 

- **A modern C++17 compiler**. I've used **GCC 9.2**. On Windows I'd recommend MinGW-w64 8.0 (an up to date one can be obtained through MSYS2).

- **SDL2** - set SDL2_DIR in the CMakeLists.txt file to the directory where the SDL2 development libraries are located on your system.

- **CMake**

TODO: add notes on how to build

A compiled release is provided in the [releases section](https://github.com/dominikrys/chip-8-emulator/releases).

## Usage

In terminal/command line run: `chip_8.exe --rom <path> [options]`

For more help run `chip_8.exe --help`

The cycle delay and operation modes may need to be changed between ROMs to ensure they work as intended. I've included 3 different operation modes due different ROMs relying on different opcode behaviours, depending on the time they were written. Explanations can be found in the links section. They are as follows:

- **CHIP8**: FX55 and FX65 opcodes increment the instruction counter. 8XY6 and 8XYE registers shift the value in VY and store the result in VX.

- **CHIP-48**: FX55 and FX65 opcodes increment the instruction counter.

- **SCHIP**: FX55 and FX65 opcodes don't increment the instruction counter (like on the SCHIP). This is what most ROMs expect, and is the default mode. The emulator doesn't actually support SCHIP opcodes (yet?).

## ROMs

- [Revival Studios CHIP-8 Program Pack](https://github.com/dmatlack/chip8/tree/master/roms)

- [CHIP-8 game archive](https://johnearnest.github.io/chip8Archive/)

- [BestCoder's test ROM](https://github.com/daniel5151/AC8E/blob/master/roms/bc_test.ch8)

## Links

- [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

- [Explanation of differing opcode behaviours on CHIP8/CHIP-48/SCHIP](https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/)

- [Explanation on FX55 & FX65 opcodes](https://github.com/Chromatophore/HP48-Superchip/blob/master/investigations/quirk_i.md)