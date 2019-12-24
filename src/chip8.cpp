#include "chip8.h"

#include <fstream>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    clearScreen();

    // Clear stack
    for (unsigned short &i : stack)
    {
        i = 0;
    }

    // Clear registers
    for (unsigned char &i : V)
    {
        i = 0;
    }

    // Clear memory
    for (unsigned char &i : memory)
    {
        i = 0;
    }

    // Clear keys
    for (unsigned char &i : key)
    {
        i = 0;
    }

    // Load font set
    for (int i = 0; i < 80; i++)
    {
        memory[i] = fontSet[i]; // TODO: This should be offset by 0x50
    }

    // Reset timers
    delayTimer = 0;
    soundTimer = 0;

    randByte = std::uniform_int_distribution<uint8_t>(0, 255u);
};

void Chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode Opcode
    switch (opcode & 0xF000)
    {
        case 0x0000: // NOTE: opcode 0NNN excluded as we're not emulating an RCA 1802
            switch (opcode & 0x000F)
            {
                case 0x000: // 0x00E0: Clears the screen.
                    clearScreen();
                    pc += 2;
                    break;
                case 0x000E: // 0x00EE: Returns from subroutine.
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
            }
            break;
        case 0x2000: // 2NNN: Calls subroutine at NNN.
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 3XNN: Skips the next instruction if VX equals NN.
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x4000: // 4XNN: Skips the next instruction if VX doesn't equal NN.
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY.
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x6000: // 6XNN: Sets VX to NN.
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000: // 7XNN: Adds NN to VX. (Carry flag is not changed).
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0000: // 8XY0: Sets VX to the value of VY.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001: // 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002: // 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003: // 8XY3: Sets VX to VX xor VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) // TODO: make this neater
                    {
                        V[0xF] = 1; // carry
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                    {
                        V[0xF] = 0; // borrow
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00u) >> 8] >>= 1;
                    pc += 2;
                    break;
                case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                    {
                        V[0xF] = 0; // borrow
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E: // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
                    break;
            }
            break;
        case 0x9000: // 9XY0: Skips the next instruction if VX doesn't equal VY.
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000: // BNNN: Jumps to the address NNN plus V0.
            pc = (opcode & 0x0FFF) + V[0];
            pc += 2;
            break;
        case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            V[(opcode & 0x0F00) >> 8] = randByte(randGen) & (opcode & 0x00FF);;
            pc += 2;
            break;
        case 0xD000:
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) // 8 is the width of the sprite
                {
                    if ((pixel & (0x80 >> xline)) != 0) // go pixel by pixel checking if it's 0
                    {
                        if (gfx[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            V[0xF] = 1; // set VF to 1 (collision detection)
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1; // set value in gfx by XORing with 1
                    }
                }
            }

            drawFlag_ = true;
            pc += 2;
        }
            break;
        case 0xE000:
            switch (opcode & 0x00FF)
            {
                case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                    {
                        pc += 4;
                    }
                    else
                    {
                        pc += 2;
                    }
                    break;
                case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                    {
                        pc += 4;
                    }
                    else
                    {
                        pc += 2;
                    }
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007: // FX07: Sets VX to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delayTimer;
                    pc += 2;
                    break;
                case 0x00A: // FX0A: A key press is awaited, and then stored in VX
                {
                    bool keyPress = false;
                    for (int i = 0; i < 16; i++)
                    {
                        if (key[i] != 0)
                        {
                            V[(opcode & 0x0F00) >> 8] = i;
                            keyPress = true;
                        }
                    }

                    if (keyPress)
                    {
                        pc += 2; // Only increment pc if a keypress received
                    }
                }
                    break;
                case 0x0015: // FX15: Sets the delay timer to VX.
                    delayTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0018: // FX18: Sets the sound timer to VX.
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x001E: // FX1E: Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't
                    if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) // TODO: make this nicer
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX.
                    I = V[(opcode & 0x0F00) >> 8] * 0x5; // TODO: offset correctly
                    pc += 2;
                    break;
                case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055: // FX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        memory[I + i] = V[i];
                    }
                    // SCHIP implementations ignores VY unlike the original. Games still work.
                    // I += ((opcode & 0x0F00) >> 8) + 1; TODO:  add a mode to enable these
                    pc += 2;
                    break;
                case 0X0065: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        V[i] = memory[I + i];
                    }
                    // SCHIP implementations ignores VY unlike the original. Games still work.
                    // I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
            }
            break;
        default:
            std::cout << "Unknown opcode: 0x" + opcode;
    }

    // Update timers
    if (delayTimer > 0)
    {
        delayTimer--;
    }

    if (soundTimer > 0)
    {
        if (soundTimer == 1)
        {
            std::cout << "BEEP";
        }
        soundTimer--;
    }
}

void Chip8::loadGame(const std::string &filepath) {
    // TODO: add more error checking. Result type.
    std::ifstream ifs(filepath, std::ios::binary);
    if (!ifs)
    {
        throw std::runtime_error("Can't open file: " + filepath + ". " + std::strerror(errno));
    }

    ifs.seekg(0, std::ios::end);
    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    auto size = std::size_t(end - ifs.tellg());
    if (size == 0)
    {
        throw std::runtime_error("No game loaded");
    }
    else if (size > sizeof(memory) / sizeof(*memory))
    {
        throw std::runtime_error("ROM too big!");
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    for (int i = 0; i < size; i++)
    {
        memory[i + 512] = buffer[i];
    }

    ifs.close();
}

bool Chip8::drawFlag() {
    return drawFlag_;
}

void Chip8::renderConsole() {
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if (gfx[(y * 64) + x] == 0)
            {
                std::cout << "O";
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Chip8::disableDrawFlag() {
    drawFlag_ = false;
}

void Chip8::clearScreen() {
    for (unsigned char &i : gfx)
    {
        i = 0;
    }

    drawFlag_ = true;
}