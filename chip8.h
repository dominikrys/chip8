#pragma once

using opcode = unsigned short;

class Chip8
{
public:
    void initialize();
    void emulateCycle();

private:
    unsigned char memory_[4096];
    unsigned char V_[16]; // Registers

    unsigned short I_; // Index register
    unsigned short pc_; // Program counter

    unsigned char gfx_[64 * 32]; // Screen

    unsigned char delayTimer_;
    unsigned char soundTimer_;

    unsigned short stack_[16];
    unsigned short sp_;

    unsigned char key_[16];
};

/*
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
 */