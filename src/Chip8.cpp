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

Chip8::Chip8(bool altOp) : pc_{0x200u}, opcode_{0}, index_{0}, sp_{0}, drawFlag_{true}, soundFlag_{false},
                           delayTimer_{0}, soundTimer_{0}, altOp_{altOp},
                           randGen_(std::chrono::system_clock::now().time_since_epoch().count()) {
    std::fill_n(stack_, STACK_SIZE, 0);
    std::fill_n(registers_, REGISTER_COUNT, 0);
    std::fill_n(memory_, MEMORY_SIZE, 0);
    std::fill_n(keys_, KEY_COUNT, 0);

    clearScreen();

    // Load font set
    for (int i = 0; i < FONT_SET_SIZE; i++)
    {
        memory_[i + FONT_SET_START_ADDRESS] = fontSet_[i];
    }

    randByte_ = std::uniform_int_distribution<uint8_t>(std::numeric_limits<uint8_t>::min(),
                                                       std::numeric_limits<uint8_t>::max());
}

void Chip8::cycle() {
    // Fetch Opcode
    opcode_ = memory_[pc_] << 8u | memory_[pc_ + 1u];

    // Decode Opcode
    switch (opcode_ & 0xF000u)
    {
        case 0x0000u: // NOTE: opcode 0NNN excluded as we're not emulating an RCA 1802 chip
            switch (opcode_ & 0x000Fu)
            {
                case 0x000u: // 0x00E0: Clears the screen
                    clearScreen();
                    drawFlag_ = true;
                    pc_ += 2;
                    break;
                case 0x000Eu: // 0x00EE: Returns from subroutine
                    sp_--;
                    pc_ = stack_[sp_];
                    pc_ += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode_];
            }
            break;
        case 0x1000u: // 1NNN: Jumps to address NNN
            pc_ = opcode_ & 0x0FFFu;
            break;
        case 0x2000u: // 2NNN: Calls subroutine at NNN
            stack_[sp_] = pc_;
            sp_++;
            pc_ = opcode_ & 0x0FFFu;
            break;
        case 0x3000u: // 3XNN: Skips the next instruction if VX equals NN.
            if (registers_[(opcode_ & 0x0F00u) >> 8u] == (opcode_ & 0x00FFu))
            {
                pc_ += 4;
            }
            else
            {
                pc_ += 2;
            }
            break;
        case 0x4000u: // 4XNN: Skips the next instruction if VX doesn't equal NN
            if (registers_[(opcode_ & 0x0F00u) >> 8u] != (opcode_ & 0x00FFu))
            {
                pc_ += 4;
            }
            else
            {
                pc_ += 2;
            }
            break;
        case 0x5000u: // 5XY0: Skips the next instruction if VX equals VY
            if (registers_[(opcode_ & 0x0F00u) >> 8u] == registers_[(opcode_ & 0x00F0u) >> 4u])
            {
                pc_ += 4;
            }
            else
            {
                pc_ += 2;
            }
            break;
        case 0x6000u: // 6XNN: Sets VX to NN
            registers_[(opcode_ & 0x0F00u) >> 8u] = opcode_ & 0x00FFu;
            pc_ += 2;
            break;
        case 0x7000u: // 7XNN: Adds NN to VX. (Carry flag is not changed)
            registers_[(opcode_ & 0x0F00u) >> 8u] += (opcode_ & 0x00FFu);
            pc_ += 2;
            break;
        case 0x8000u:
            switch (opcode_ & 0x000Fu)
            {
                case 0x0000u: // 8XY0: Sets VX to the value of VY
                    registers_[(opcode_ & 0x0F00u) >> 8u] = registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0001u: // 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
                    registers_[(opcode_ & 0x0F00u) >> 8u] |= registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0002u: // 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
                    registers_[(opcode_ & 0x0F00u) >> 8u] &= registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0003u: // 8XY3: Sets VX to VX xor VY
                    registers_[(opcode_ & 0x0F00u) >> 8u] ^= registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0004u: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
                    // TODO: make this neater
                    if (registers_[(opcode_ & 0x00F0u) >> 4u] > (0xFFu - registers_[(opcode_ & 0x0F00u) >> 8u]))
                    {
                        registers_[0xF] = 1; // Carry
                    }
                    else
                    {
                        registers_[0xF] = 0;
                    }
                    registers_[(opcode_ & 0x0F00u) >> 8u] += registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0005u: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    if (registers_[(opcode_ & 0x00F0u) >> 4u] > registers_[(opcode_ & 0x0F00u) >> 8u])
                    {
                        registers_[0xF] = 0; // Borrow
                    }
                    else
                    {
                        registers_[0xF] = 1;
                    }
                    registers_[(opcode_ & 0x0F00u) >> 8u] -= registers_[(opcode_ & 0x00F0u) >> 4u];
                    pc_ += 2;
                    break;
                case 0x0006u: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
                    registers_[0xF] = registers_[(opcode_ & 0x0F00u) >> 8u] & 0x1u;
                    registers_[(opcode_ & 0x0F00u) >> 8u] >>= 1u;
                    pc_ += 2;
                    break;
                case 0x0007u: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    if (registers_[(opcode_ & 0x0F00u) >> 8u] > registers_[(opcode_ & 0x00F0u) >> 4u])
                    {
                        registers_[0xF] = 0; // Borrow
                    }
                    else
                    {
                        registers_[0xF] = 1;
                    }
                    registers_[(opcode_ & 0x0F00u) >> 8u] =
                            registers_[(opcode_ & 0x00F0u) >> 4u] - registers_[(opcode_ & 0x0F00u) >> 8u];
                    pc_ += 2;
                    break;
                case 0x000Eu: // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
                    registers_[0xF] = registers_[(opcode_ & 0x0F00u) >> 8u] >> 7u;
                    registers_[(opcode_ & 0x0F00u) >> 8u] <<= 1u;
                    pc_ += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode_];
            }
            break;
        case 0x9000u: // 9XY0: Skips the next instruction if VX doesn't equal VY
            if (registers_[(opcode_ & 0x0F00u) >> 8u] != registers_[(opcode_ & 0x00F0u) >> 4u])
            {
                pc_ += 4;
            }
            else
            {
                pc_ += 2;
            }
            break;
        case 0xA000u: // ANNN: Sets I to the address NNN
            index_ = opcode_ & 0x0FFFu;
            pc_ += 2;
            break;
        case 0xB000u: // BNNN: Jumps to the address NNN plus V0
            pc_ = (opcode_ & 0x0FFFu) + registers_[0];
            pc_ += 2;
            break;
        case 0xC000u: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
            registers_[(opcode_ & 0x0F00u) >> 8u] = randByte_(randGen_) & (opcode_ & 0x00FFu);
            pc_ += 2;
            break;
        case 0xD000u:
        {
            uint8_t x = registers_[(opcode_ & 0x0F00u) >> 8u];
            uint8_t y = registers_[(opcode_ & 0x00F0u) >> 4u];
            uint8_t height = opcode_ & 0x000Fu;

            // Set VF to 0 (for collision detection)
            registers_[0xF] = 0;

            for (unsigned int row = 0; row < height; row++)
            {
                uint8_t spriteByte = memory_[index_ + row];

                for (unsigned int column = 0; column < SPRITE_WIDTH; column++)
                {
                    // Go pixel by pixel checking if it's 0
                    if (spriteByte & (0x80u >> column))
                    {
                        // Check collision. % (VIDEO_WIDTH * VIDEO_HEIGHT) is for wrapping the sprite around the screen
                        if (video_[x + column + ((y + row) * VIDEO_WIDTH) % (VIDEO_WIDTH * VIDEO_HEIGHT)] == 0xFFFFFFFF)
                        {
                            // Set VF to 1 (for collision detection)
                            registers_[0xF] = 1;
                        }
                        // Set value in video by XORing with 1
                        video_[x + column + ((y + row) * VIDEO_WIDTH) % (VIDEO_WIDTH * VIDEO_HEIGHT)] ^= 0xFFFFFFFF;
                    }
                }
            }

            drawFlag_ = true;
            pc_ += 2;
        }
            break;
        case 0xE000u:
            switch (opcode_ & 0x00FFu)
            {
                case 0x009Eu: // EX9E: Skips the next instruction if the key stored in VX is pressed
                    if (keys_[registers_[(opcode_ & 0x0F00u) >> 8u]])
                    {
                        pc_ += 4;
                    }
                    else
                    {
                        pc_ += 2;
                    }
                    break;
                case 0x00A1u: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    if (keys_[registers_[(opcode_ & 0x0F00u) >> 8u]] == 0)
                    {
                        pc_ += 4;
                    }
                    else
                    {
                        pc_ += 2;
                    }
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode_];
            }
            break;
        case 0xF000u:
            switch (opcode_ & 0x00FFu)
            {
                case 0x0007u: // FX07: Sets VX to the value of the delay timer
                    registers_[(opcode_ & 0x0F00u) >> 8u] = delayTimer_;
                    pc_ += 2;
                    break;
                case 0x00Au: // FX0A: A key press is awaited, and then stored in VX
                {
                    bool keyPress = false;
                    for (int i = 0; i < KEY_COUNT; i++)
                    {
                        if (keys_[i])
                        {
                            registers_[(opcode_ & 0x0F00u) >> 8u] = i;
                            keyPress = true;
                        }
                    }

                    if (!keyPress)
                    {
                        return;
                    }

                    pc_ += 2; // Only increment pc if a key is pressed
                }
                    break;
                case 0x0015u: // FX15: Sets the delay timer to VX
                    delayTimer_ = registers_[(opcode_ & 0x0F00u) >> 8u];
                    pc_ += 2;
                    break;
                case 0x0018u: // FX18: Sets the sound timer to VX
                    soundTimer_ = registers_[(opcode_ & 0x0F00u) >> 8u];
                    pc_ += 2;
                    break;
                case 0x001Eu: // FX1E: Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't
                    // TODO: make this nicer
                    if (index_ + registers_[(opcode_ & 0x0F00u) >> 8u] > 0xFFFu)
                    {
                        registers_[0xF] = 1;
                    }
                    else
                    {
                        registers_[0xF] = 0;
                    }
                    index_ += registers_[(opcode_ & 0x0F00u) >> 8u];
                    pc_ += 2;
                    break;
                case 0x0029u: // FX29: Sets I to the location of the sprite for the character in VX
                    index_ = FONT_SET_START_ADDRESS + registers_[(opcode_ & 0x0F00u) >> 8u] * 0x5u;
                    pc_ += 2;
                    break;
                case 0x0033u: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
                    memory_[index_] = registers_[(opcode_ & 0x0F00u) >> 8u] / 100;
                    memory_[index_ + 1] = (registers_[(opcode_ & 0x0F00u) >> 8u] / 10) % 10;
                    memory_[index_ + 2] = (registers_[(opcode_ & 0x0F00u) >> 8u] % 100) % 10;
                    pc_ += 2;
                    break;
                case 0x0055u: // FX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                    for (int i = 0; i <= ((opcode_ & 0x0F00u) >> 8u); i++)
                    {
                        memory_[index_ + i] = registers_[i];
                    }

                    // In CHIP-8 the value of VY should be shifted and stored in VX, however since the S-CHIP and
                    // CHIP-48 implementations don't do this, I will omit it and won't include a flag to set it. Most
                    // ROMs rely on this opcode to function like on the CHIP-48.

                    if (altOp)
                    {
                        // Increment index like on CHIP-48. The majority of ROMs don't rely on this, however since some
                        // do I've given the option to enable it. It should be disabled for SCHIP games.
                        index_ += ((opcode_ & 0x0F00u) >> 8u) + 1;
                    }
                    pc_ += 2;
                    break;
                case 0x0065u: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I
                    for (int i = 0; i <= ((opcode_ & 0x0F00u) >> 8u); i++)
                    {
                        registers_[i] = memory_[index_ + i];
                    }

                    // VY ignored. Same reasoning for this as above for FX55

                    if (altOp)
                    {
                        // VY ignored. Same reasoning for this as above for FX55
                        index_ += ((opcode_ & 0x0F00u) >> 8u) + 1;
                    }
                    pc_ += 2;
                    break;
                default:;
                    std::cout << &"Unknown opcode: 0x"[opcode_];
            }
            break;
        default:;
            std::cout << &"Unknown opcode: 0x"[opcode_];
    }

    // Update timers
    if (delayTimer_ > 0)
    {
        delayTimer_--;
    }

    if (soundTimer_ > 0)
    {
        if (soundTimer_ == 1)
        {
            soundFlag_ = true;
        }
        soundTimer_--;
    }
}

void Chip8::loadRom(const std::string &filepath) {
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if (!ifs)
    {
        throw std::runtime_error("Can't open file: " + filepath + ". " + std::strerror(errno));
    }

    auto end = ifs.tellg(); // ifs is at the end since std::ios::ate specified
    ifs.seekg(0, std::ios::beg);
    auto size = std::size_t(end - ifs.tellg());
    if (size == 0)
    {
        throw std::runtime_error("Specified ROM has a size of 0.");
    }
    else if (size > MEMORY_SIZE - ROM_START_ADDRESS)
    {
        throw std::runtime_error("ROM too big for memory");
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    for (int i = 0; i < size; i++)
    {
        memory_[i + ROM_START_ADDRESS] = buffer[i];
    }

    ifs.close();
}

bool Chip8::drawFlag() {
    return drawFlag_;
}

void Chip8::disableDrawFlag() {
    drawFlag_ = false;
}

void Chip8::clearScreen() {
    std::fill_n(video_, VIDEO_WIDTH * VIDEO_HEIGHT, 0);
}

uint32_t *Chip8::video() {
    return video_;
}

uint8_t *Chip8::keys() {
    return keys_;
}

bool Chip8::soundFlag() {
    return soundFlag_;
}

void Chip8::disableSoundFlag() {
    soundFlag_ = false;
}
