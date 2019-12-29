#include "Chip8.h"

#include <fstream>
#include <cstddef>
#include <cstring>
#include <vector>
#include <chrono>
#include <iostream>
#include <limits>

const unsigned int SPRITE_WIDTH = 8;
const unsigned int ROM_START_ADDRESS = 0x200;
const unsigned int FONT_SET_START_ADDRESS = 0x050;

Chip8::Chip8() : pc{0x200u}, opcode{0}, index{0}, sp{0}, drawFlag{true}, soundFlag{false}, delayTimer{0}, soundTimer{0},
                 randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    clearScreen();

    std::fill_n(stack, STACK_SIZE, 0);
    std::fill_n(registers, REGISTER_COUNT, 0);
    std::fill_n(memory, MEMORY_SIZE, 0);
    std::fill_n(keys, KEY_COUNT, 0);

    // Load font set
    for (int i = 0; i < FONT_SET_SIZE; i++)
    {
        memory[i + FONT_SET_START_ADDRESS] = fontSet[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(std::numeric_limits<uint8_t>::min(),
                                                      std::numeric_limits<uint8_t>::max());
}

void Chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8u | memory[pc + 1u];

    // Decode Opcode
    switch (opcode & 0xF000u)
    {
        case 0x0000u: // NOTE: opcode 0NNN excluded as we're not emulating an RCA 1802 chip
            switch (opcode & 0x000Fu)
            {
                case 0x000u: // 0x00E0: Clears the screen.
                    clearScreen();
                    drawFlag = true;
                    pc += 2;
                    break;
                case 0x000Eu: // 0x00EE: Returns from subroutine.
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode];
            }
            break;
        case 0x1000u: // 1NNN: Jumps to address NNN
            pc = opcode & 0x0FFFu;
            break;
        case 0x2000u: // 2NNN: Calls subroutine at NNN.
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFFu;
            break;
        case 0x3000u: // 3XNN: Skips the next instruction if VX equals NN.
            if (registers[(opcode & 0x0F00u) >> 8u] == (opcode & 0x00FFu))
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x4000u: // 4XNN: Skips the next instruction if VX doesn't equal NN.
            if (registers[(opcode & 0x0F00u) >> 8u] != (opcode & 0x00FFu))
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x5000u: // 5XY0: Skips the next instruction if VX equals VY.
            if (registers[(opcode & 0x0F00u) >> 8u] == registers[(opcode & 0x00F0u) >> 4u])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x6000u: // 6XNN: Sets VX to NN.
            registers[(opcode & 0x0F00u) >> 8u] = opcode & 0x00FFu;
            pc += 2;
            break;
        case 0x7000u: // 7XNN: Adds NN to VX. (Carry flag is not changed).
            registers[(opcode & 0x0F00u) >> 8u] += (opcode & 0x00FFu);
            pc += 2;
            break;
        case 0x8000u:
            switch (opcode & 0x000Fu)
            {
                case 0x0000u: // 8XY0: Sets VX to the value of VY.
                    registers[(opcode & 0x0F00u) >> 8u] = registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0001u: // 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
                    registers[(opcode & 0x0F00u) >> 8u] |= registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0002u: // 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
                    registers[(opcode & 0x0F00u) >> 8u] &= registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0003u: // 8XY3: Sets VX to VX xor VY
                    registers[(opcode & 0x0F00u) >> 8u] ^= registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0004u: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    if (registers[(opcode & 0x00F0u) >> 4u] >
                        (0xFFu - registers[(opcode & 0x0F00u) >> 8u])) // TODO: make this neater
                    {
                        registers[0xF] = 1; // carry
                    }
                    else
                    {
                        registers[0xF] = 0;
                    }
                    registers[(opcode & 0x0F00u) >> 8u] += registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0005u: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if (registers[(opcode & 0x00F0u) >> 4u] > registers[(opcode & 0x0F00u) >> 8u])
                    {
                        registers[0xF] = 0; // borrow
                    }
                    else
                    {
                        registers[0xF] = 1;
                    }
                    registers[(opcode & 0x0F00u) >> 8u] -= registers[(opcode & 0x00F0u) >> 4u];
                    pc += 2;
                    break;
                case 0x0006u: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
                    registers[0xF] = registers[(opcode & 0x0F00u) >> 8u] & 0x1u;
                    registers[(opcode & 0x0F00u) >> 8u] >>= 1u;
                    pc += 2;
                    break;
                case 0x0007u: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if (registers[(opcode & 0x0F00u) >> 8u] > registers[(opcode & 0x00F0u) >> 4u])
                    {
                        registers[0xF] = 0; // borrow
                    }
                    else
                    {
                        registers[0xF] = 1;
                    }
                    registers[(opcode & 0x0F00u) >> 8u] =
                            registers[(opcode & 0x00F0u) >> 4u] - registers[(opcode & 0x0F00u) >> 8u];
                    pc += 2;
                    break;
                case 0x000Eu: // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                    registers[0xF] = registers[(opcode & 0x0F00u) >> 8u] >> 7u;
                    registers[(opcode & 0x0F00u) >> 8u] <<= 1u;
                    pc += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode];
            }
            break;
        case 0x9000u: // 9XY0: Skips the next instruction if VX doesn't equal VY.
            if (registers[(opcode & 0x0F00u) >> 8u] != registers[(opcode & 0x00F0u) >> 4u])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0xA000u: // ANNN: Sets I to the address NNN
            index = opcode & 0x0FFFu;
            pc += 2;
            break;
        case 0xB000u: // BNNN: Jumps to the address NNN plus V0.
            pc = (opcode & 0x0FFFu) + registers[0];
            pc += 2;
            break;
        case 0xC000u: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            registers[(opcode & 0x0F00u) >> 8u] = randByte(randGen) & (opcode & 0x00FFu);
            pc += 2;
            break;
        case 0xD000u:
        {
            uint8_t x = registers[(opcode & 0x0F00u) >> 8u];
            uint8_t y = registers[(opcode & 0x00F0u) >> 4u];
            uint8_t height = opcode & 0x000Fu;

            registers[0xF] = 0; // set VF to 0 (collision detection)

            for (unsigned int row = 0; row < height; row++)
            {
                uint8_t spritePixel = memory[index + row];
                for (unsigned int column = 0; column < SPRITE_WIDTH; column++)
                {
                    if (spritePixel & (0x80u >> column)) // go pixel by pixel checking if it's 0
                    {
                        // TODO: look at wraparound - works for most roms?
                        if (video[(x + column + ((y + row) * VIDEO_WIDTH))] == 0xFFFFFFFF)
                        {
                            registers[0xF] = 1; // set VF to 1 (collision detection)
                        }
                        // set value in video by XORing with 1
                        video[x + column + ((y + row) * VIDEO_WIDTH) % (VIDEO_WIDTH * VIDEO_HEIGHT)] ^= 0xFFFFFFFF;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
            break;
        case 0xE000u:
            switch (opcode & 0x00FFu)
            {
                case 0x009Eu: // EX9E: Skips the next instruction if the key stored in VX is pressed
                    if (keys[registers[(opcode & 0x0F00u) >> 8u]])
                    {
                        pc += 4;
                    }
                    else
                    {
                        pc += 2;
                    }
                    break;
                case 0x00A1u: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    if (keys[registers[(opcode & 0x0F00u) >> 8u]] == 0)
                    {
                        pc += 4;
                    }
                    else
                    {
                        pc += 2;
                    }
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode];
            }
            break;
        case 0xF000u:
            switch (opcode & 0x00FFu)
            {
                case 0x0007u: // FX07: Sets VX to the value of the delay timer
                    registers[(opcode & 0x0F00u) >> 8u] = delayTimer;
                    pc += 2;
                    break;
                case 0x00Au: // FX0A: A key press is awaited, and then stored in VX
                {
                    bool keyPress = false;
                    for (int i = 0; i < KEY_COUNT; i++)
                    {
                        if (keys[i])
                        {
                            registers[(opcode & 0x0F00u) >> 8u] = i;
                            keyPress = true;
                        }
                    }

                    if (!keyPress)
                    {
                        return;
                    }

                    pc += 2; // Only increment pc if a key is pressed
                }
                    break;
                case 0x0015u: // FX15: Sets the delay timer to VX.
                    delayTimer = registers[(opcode & 0x0F00u) >> 8u];
                    pc += 2;
                    break;
                case 0x0018u: // FX18: Sets the sound timer to VX.
                    soundTimer = registers[(opcode & 0x0F00u) >> 8u];
                    pc += 2;
                    break;
                case 0x001Eu: // FX1E: Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't
                    // TODO: make this nicer
                    if (index + registers[(opcode & 0x0F00u) >> 8u] > 0xFFFu)
                    {
                        registers[0xF] = 1;
                    }
                    else
                    {
                        registers[0xF] = 0;
                    }
                    index += registers[(opcode & 0x0F00u) >> 8u];
                    pc += 2;
                    break;
                case 0x0029u: // FX29: Sets I to the location of the sprite for the character in VX.
                    index = FONT_SET_START_ADDRESS + registers[(opcode & 0x0F00u) >> 8u] * 0x5u;
                    pc += 2;
                    break;
                case 0x0033u: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
                    memory[index] = registers[(opcode & 0x0F00u) >> 8u] / 100;
                    memory[index + 1] = (registers[(opcode & 0x0F00u) >> 8u] / 10) % 10;
                    memory[index + 2] = (registers[(opcode & 0x0F00u) >> 8u] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055u: // FX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                    for (int i = 0; i <= ((opcode & 0x0F00u) >> 8u); i++)
                    {
                        memory[index + i] = registers[i];
                    }
                    // S-CHIP implementations ignores VY unlike the original. Games still work.
                    // I += ((opcode & 0x0F00) >> 8) + 1;
                    // TODO: add a 'quirk' mode to enable these + the Y shift
                    pc += 2;
                    break;
                case 0x0065u: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
                    for (int i = 0; i <= ((opcode & 0x0F00u) >> 8u); i++)
                    {
                        registers[i] = memory[index + i];
                    }
                    // S-CHIP implementations ignores VY unlike the original. Games still work.
                    // I += ((opcode & 0x0F00) >> 8) + 1;
                    // TODO: add a mode to enable these
                    pc += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode];
            }
            break;
        default:;
            std::cout << &"Unknown opcode: 0x"[opcode];
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
            soundFlag = true;
            // std::cout << "BEEP"; TODO: add this back in - maybe log when audo disabled?
        }
        soundTimer--;
    }
}

void Chip8::loadRom(const std::string &filepath) {
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
    else if (size > MEMORY_SIZE - ROM_START_ADDRESS)
    {
        throw std::runtime_error("Error: ROM too big for memory");
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    for (int i = 0; i < size; i++)
    {
        memory[i + ROM_START_ADDRESS] = buffer[i];
    }

    ifs.close();
}

bool Chip8::getDrawFlag() {
    return drawFlag;
}

void Chip8::disableDrawFlag() {
    drawFlag = false;
}

void Chip8::clearScreen() {
    std::fill_n(video, VIDEO_WIDTH * VIDEO_HEIGHT, 0);
}

uint32_t *Chip8::getVideo() {
    return video;
}

uint8_t *Chip8::getKeys() {
    return keys;
}

bool Chip8::getSoundFlag() {
    return soundFlag;
}

void Chip8::disableSoundFlag() {
    soundFlag = false;
}
