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
const unsigned int CHARACTER_SPRITE_WIDTH = 0x5;

Chip8::Chip8(Mode mode)
        : memory_{},
          registers_{},
          opcode_{0},
          index_{0},
          pc_{0x200u},
          stack_{},
          sp_{0},
          video_{},
          delayTimer_{0},
          soundTimer_{0},
          keys_{},
          drawFlag_{true},
          soundFlag_{false},
          mode_{mode},
          randGen_(std::chrono::system_clock::now().time_since_epoch().count()) {
    std::fill_n(stack_, STACK_SIZE, 0);
    std::fill_n(registers_, REGISTER_COUNT, 0);
    std::fill_n(memory_, MEMORY_SIZE, 0);
    std::fill_n(keys_, KEY_COUNT, 0);

    randByte_ = std::uniform_int_distribution<uint8_t>(std::numeric_limits<uint8_t>::min(),
                                                       std::numeric_limits<uint8_t>::max());

    clearScreen();

    fontSet_ = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Load font set into memory
    for (unsigned int i = 0; i < FONT_SET_SIZE; i++)
    {
        memory_[i + FONT_SET_START_ADDRESS] = fontSet_[i];
    }

    // Set up function pointer table
    funcTable_[0x0] = &Chip8::decodeFuncTable0;
    funcTable_[0x1] = &Chip8::opcode1NNN;
    funcTable_[0x2] = &Chip8::opcode2NNN;
    funcTable_[0x3] = &Chip8::opcode3XNN;
    funcTable_[0x4] = &Chip8::opcode4XNN;
    funcTable_[0x5] = &Chip8::opcode5XY0;
    funcTable_[0x6] = &Chip8::opcode6XNN;
    funcTable_[0x7] = &Chip8::opcode7XNN;
    funcTable_[0x8] = &Chip8::decodeFuncTable8;
    funcTable_[0x9] = &Chip8::opcode9XY0;
    funcTable_[0xA] = &Chip8::opcodeANNN;
    funcTable_[0xB] = &Chip8::opcodeBNNN;
    funcTable_[0xC] = &Chip8::opcodeCXNN;
    funcTable_[0xD] = &Chip8::opcodeDXYN;
    funcTable_[0xE] = &Chip8::decodeFuncTableE;
    funcTable_[0xF] = &Chip8::decodeFuncTableF;

    funcTable0_[0x0] = &Chip8::opcode00E0;
    funcTable0_[0xE] = &Chip8::opcode00EE;

    funcTable8_[0x0] = &Chip8::opcode8XY0;
    funcTable8_[0x1] = &Chip8::opcode8XY1;
    funcTable8_[0x2] = &Chip8::opcode8XY2;
    funcTable8_[0x3] = &Chip8::opcode8XY3;
    funcTable8_[0x4] = &Chip8::opcode8XY4;
    funcTable8_[0x5] = &Chip8::opcode8XY5;
    funcTable8_[0x6] = &Chip8::opcode8XY6;
    funcTable8_[0x7] = &Chip8::opcode8XY7;
    funcTable8_[0xE] = &Chip8::opcode8XYE;

    funcTableE_[0x1] = &Chip8::opcodeEXA1;
    funcTableE_[0xE] = &Chip8::opcodeEX9E;

    funcTableF_[0x07] = &Chip8::opcodeFX07;
    funcTableF_[0x0A] = &Chip8::opcodeFX0A;
    funcTableF_[0x15] = &Chip8::opcodeFX15;
    funcTableF_[0x18] = &Chip8::opcodeFX18;
    funcTableF_[0x1E] = &Chip8::opcodeFX1E;
    funcTableF_[0x29] = &Chip8::opcodeFX29;
    funcTableF_[0x33] = &Chip8::opcodeFX33;
    funcTableF_[0x55] = &Chip8::opcodeFX55;
    funcTableF_[0x65] = &Chip8::opcodeFX65;

}

void Chip8::cycle() {
    // Fetch Opcode
    opcode_ = memory_[pc_] << 8u | memory_[pc_ + 1u];

    // Decode and execute opcode
    ((*this).*(funcTable_[(opcode_ & 0xF000u) >> 12u]))();

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

void Chip8::decodeFuncTable0() {
    // Only the last bit is different in 0x0 opcodes
    ((*this).*(funcTable0_[(opcode_ & 0x000Fu)]))();
}

void Chip8::decodeFuncTable8() {
    // Only the last bit is different in 0x8 opcodes
    ((*this).*(funcTable8_[(opcode_ & 0x000Fu)]))();
}

void Chip8::decodeFuncTableE() {
    // Only the last bit is different in 0xE opcodes
    ((*this).*(funcTableE_[(opcode_ & 0x000Fu)]))();
}

void Chip8::decodeFuncTableF() {
    // The last two bits are different in in 0xF opcodes
    ((*this).*(funcTableF_[(opcode_ & 0x00FFu)]))();
}

void Chip8::opcodeUnknown() {
    std::cout << &"Unknown opcode: 0x"[opcode_];
}

// NOTE: opcode 0NNN is excluded as we're not emulating an RCA 1802 chip

// 0x00E0: Clears the screen
void Chip8::opcode00E0() {
    clearScreen();
    drawFlag_ = true;
    pc_ += 2;
}

// 0x00EE: Returns from subroutine
void Chip8::opcode00EE() {
    // Restore pc from stack
    sp_--;
    pc_ = stack_[sp_];
    pc_ += 2;
}

// 1NNN: Jumps to address NNN
void Chip8::opcode1NNN() {
    auto address = opcode_ & 0x0FFFu;

    pc_ = address;
}

// 2NNN: Calls subroutine at address NNN
void Chip8::opcode2NNN() {
    // Put current pc onto stack
    stack_[sp_] = pc_;
    sp_++;

    auto address = opcode_ & 0x0FFFu;
    pc_ = address;
}

// 3XNN: Skips the next instruction if VX equals NN
void Chip8::opcode3XNN() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto nn = opcode_ & 0x00FFu;

    if (registers_[x] == nn)
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// 4XNN: Skips the next instruction if VX doesn't equal NN
void Chip8::opcode4XNN() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto nn = opcode_ & 0x00FFu;

    if (registers_[x] != nn)
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// 5XY0: Skips the next instruction if VX equals VY
void Chip8::opcode5XY0() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    if (registers_[x] == registers_[y])
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// 6XNN: Sets VX to NN
void Chip8::opcode6XNN() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto nn = opcode_ & 0x00FFu;

    registers_[x] = nn;
    pc_ += 2;
}

// 7XNN: Adds NN to VX. (Carry flag is not changed)
void Chip8::opcode7XNN() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto nn = opcode_ & 0x00FFu;

    registers_[x] += nn;
    pc_ += 2;
}

// 8XY0: Sets VX to the value of VY
void Chip8::opcode8XY0() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    registers_[x] = registers_[y];
    pc_ += 2;
}

// 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
void Chip8::opcode8XY1() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    registers_[x] |= registers_[y];
    pc_ += 2;
}

// 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
void Chip8::opcode8XY2() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    registers_[x] &= registers_[y];
    pc_ += 2;
}

// 8XY3: Sets VX to VX xor VY
void Chip8::opcode8XY3() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    registers_[x] ^= registers_[y];
    pc_ += 2;
}

// 8XY4: Adds VY to VX. VF is set to 1 when there's a carry (VY + VX > 0xFFFu), and to 0 when there isn't
void Chip8::opcode8XY4() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    if (registers_[x] + registers_[y] > 0xFFFu)
    {
        registers_[0xF] = 1; // Carry
    }
    else
    {
        registers_[0xF] = 0;
    }

    registers_[x] += registers_[y];
    pc_ += 2;
}

// 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
void Chip8::opcode8XY5() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    if (registers_[y] > registers_[x])
    {
        registers_[0xF] = 0; // Borrow
    }
    else
    {
        registers_[0xF] = 1;
    }

    registers_[x] -= registers_[y];
    pc_ += 2;
}

// 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
void Chip8::opcode8XY6() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    registers_[0xF] = registers_[x] & 0x1u;

    if (mode_ == Mode::CHIP8)
    {
        // On CHIP8, shift VY and store the result in VX.
        // See: https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/
        auto y = (opcode_ & 0x00F0u) >> 4u;
        registers_[x] = registers_[y] >> 1u;
    }
    else
    {
        registers_[x] >>= 1u;
    }

    pc_ += 2;
}

// 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
void Chip8::opcode8XY7() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    if (registers_[x] > registers_[(opcode_ & 0x00F0u) >> 4u])
    {
        registers_[0xF] = 0; // Borrow
    }
    else
    {
        registers_[0xF] = 1;
    }

    registers_[x] = registers_[y] - registers_[x];
    pc_ += 2;
}

// 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
void Chip8::opcode8XYE() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    registers_[0xF] = registers_[x] >> 7u;

    if (mode_ == Mode::CHIP8)
    {
        // Check comment above for 8XY6 for an explanation why VY is shifted
        auto y = (opcode_ & 0x00F0u) >> 4u;
        registers_[x] = registers_[y] << 1u;
    }
    else
    {
        registers_[x] <<= 1u;
    }

    pc_ += 2;
}

// 9XY0: Skips the next instruction if VX doesn't equal VY
void Chip8::opcode9XY0() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto y = (opcode_ & 0x00F0u) >> 4u;

    if (registers_[x] != registers_[y])
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// ANNN: Sets I to the address NNN
void Chip8::opcodeANNN() {
    auto address = opcode_ & 0x0FFFu;

    index_ = address;
    pc_ += 2;
}

// BNNN: Jumps to the address NNN plus V0
void Chip8::opcodeBNNN() {
    auto address = opcode_ & 0x0FFFu;

    pc_ = address + registers_[0];
    pc_ += 2;
}

// CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
void Chip8::opcodeCXNN() {
    auto x = (opcode_ & 0x0F00u) >> 8u;
    auto nn = opcode_ & 0x00FFu;

    registers_[x] = randByte_(randGen_) & nn;
    pc_ += 2;
}

// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8
// pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this
// instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite
// is drawn, and to 0 if that doesn’t happen
void Chip8::opcodeDXYN() {
    auto vx = registers_[(opcode_ & 0x0F00u) >> 8u];
    auto vy = registers_[(opcode_ & 0x00F0u) >> 4u];
    auto height = opcode_ & 0x000Fu;

    // Set VF to 0 (for collision detection)
    registers_[0xF] = 0;

    for (unsigned int yLine = 0; yLine < height; yLine++)
    {
        auto spritePixel = memory_[index_ + yLine];

        for (unsigned int xLine = 0; xLine < SPRITE_WIDTH; xLine++)
        {
            // Check if sprite pixel is set to 1 (0x80 >> xline iterates through the byte one bit at a time)
            if (spritePixel & (0x80u >> xLine))
            {
                // Get pointer to pixel in video buffer.
                // "% (VIDEO_WIDTH * VIDEO_HEIGHT)" is necessary for wrapping the sprite around.
                auto *pixel = &video_[vx + xLine + ((vy + yLine) * VIDEO_WIDTH) % (VIDEO_WIDTH * VIDEO_HEIGHT)];

                // Check collision
                if (*pixel == 0xFFFFFFFF)
                {
                    // Set VF to 1 (for collision detection)
                    registers_[0xF] = 1;
                }

                // Set value in video by XORing with 1
                *pixel ^= 0xFFFFFFFF;
            }
        }
    }

    drawFlag_ = true;
    pc_ += 2;
}

// EX9E: Skips the next instruction if the key stored in VX is pressed
void Chip8::opcodeEX9E() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    if (keys_[registers_[x]])
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// EXA1: Skips the next instruction if the key stored in VX isn't pressed
void Chip8::opcodeEXA1() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    if (keys_[registers_[x]] == 0)
    {
        pc_ += 4;
    }
    else
    {
        pc_ += 2;
    }
}

// FX07: Sets VX to the value of the delay timer
void Chip8::opcodeFX07() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    registers_[x] = delayTimer_;
    pc_ += 2;
}

// FX0A: A key press is awaited, and then stored in VX
void Chip8::opcodeFX0A() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    bool keyPress = false;

    for (unsigned int i = 0; i < KEY_COUNT; i++)
    {
        if (keys_[i])
        {
            registers_[x] = i;
            keyPress = true;
        }
    }

    if (!keyPress)
    {
        // Don't increment PC, which will lead the emulator to come back to this instruction.
        return;
    }

    // Only increment pc if a key is pressed
    pc_ += 2;
}

// FX15: Sets the delay timer to VX
void Chip8::opcodeFX15() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    delayTimer_ = registers_[x];
    pc_ += 2;
}

// FX18: Sets the sound timer to VX
void Chip8::opcodeFX18() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    soundTimer_ = registers_[x];
    pc_ += 2;
}

// FX1E: Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't
void Chip8::opcodeFX1E() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    if (index_ + registers_[x] > 0xFFFu)
    {
        registers_[0xF] = 1;
    }
    else
    {
        registers_[0xF] = 0;
    }

    index_ += registers_[x];
    pc_ += 2;
}

// FX29: Sets I to the location of the sprite for the character in VX
void Chip8::opcodeFX29() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    index_ = FONT_SET_START_ADDRESS + registers_[x] * CHARACTER_SPRITE_WIDTH;
    pc_ += 2;
}

// FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
void Chip8::opcodeFX33() {
    auto vx = registers_[(opcode_ & 0x0F00u) >> 8u];

    memory_[index_] = vx / 100; // Hundreds place
    memory_[index_ + 1] = (vx / 10) % 10; // Tens place
    memory_[index_ + 2] = (vx % 100) % 10; // Ones place

    pc_ += 2;
}

// FX55: Stores V0 to VX (including VX) in memory starting at address I.
void Chip8::opcodeFX55() {
    auto x = (opcode_ & 0x0F00u) >> 8u;

    for (unsigned int i = 0; i <= x; i++)
    {
        memory_[index_ + i] = registers_[i];

        if (mode_ == Mode::CHIP8 || mode_ == Mode::CHIP48)
        {
            // On CHIP-8 and CHIP-48, the index is incremented by the number of bytes loaded or stored. Most ROMs
            // however don't assume this behaviour, so by default this is ignored (like on the SCHIP).
            // See: https://en.wikipedia.org/wiki/CHIP-8#cite_note-increment-10
            // And: https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/
            // And: https://github.com/Chromatophore/HP48-Superchip/blob/master/investigations/quirk_i.md
            index_ += registers_[i];
        }
    }

    pc_ += 2;
}

// FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
void Chip8::opcodeFX65() {
    for (unsigned int i = 0; i <= ((opcode_ & 0x0F00u) >> 8u); i++)
    {
        registers_[i] = memory_[index_ + i];

        if (mode_ == Mode::CHIP8 || mode_ == Mode::CHIP48)
        {
            // Check comment above for FX55 for an explanation why this is incremented.
            index_ += memory_[index_ + i];
        }
    }

    pc_ += 2;
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
    for (long long unsigned int i = 0; i < size; i++)
    {
        memory_[i + ROM_START_ADDRESS] = buffer[i];
    }

    ifs.close();
}

bool Chip8::drawFlag() const {
    return drawFlag_;
}

void Chip8::disableDrawFlag() {
    drawFlag_ = false;
}

void Chip8::clearScreen() {
    std::fill_n(video_, VIDEO_WIDTH * VIDEO_HEIGHT, 0);
}

const uint32_t *Chip8::video() const {
    return video_;
}

uint8_t *Chip8::keys() {
    return keys_;
}

bool Chip8::soundFlag() const {
    return soundFlag_;
}

void Chip8::disableSoundFlag() {
    soundFlag_ = false;
}