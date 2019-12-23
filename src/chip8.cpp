#include "chip8.h"

#include <fstream>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>

Chip8::Chip8() {
    initialize();
};

void Chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Clear display
    for (unsigned char &i : gfx)
    {
        i = 0;
    }

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
        memory[i] = fontSet[i];
    }

    // Reset timers
    delayTimer = 0;
    soundTimer = 0;

    drawFlag = true;
}

void Chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode Opcode
    switch (opcode & 0xF000)
    {
        case 0x0000: // Two opcodes start with 0 (excluding 0NNN)
            switch (opcode & 0x000F)
            {
                case 0x000: // 0x00E0: Clears the screen
                    // TODO: clear screen
                    break;
                case 0x000E: // 0x00EE: Returns from subroutine
                    // TODO: execute
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
                    break;
            }
            break;
        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0X0FFF;
            pc += 2;
            break;
        case 0x2000: // 2NNN: Calls subroutine at NNN
            stack[sp] = pc;
            sp++;
            pc = opcode & 0X0FFF;
            break;
        case 0x8000: // Multiple opcodes start with an 8
            switch (opcode & 0x000F)
            {
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
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: 0x" + opcode;
                    break;
            }
            break;
        default:
            std::cout << "Unknown opcode: 0x" + opcode;
            break;
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