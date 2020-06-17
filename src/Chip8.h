#pragma once

#include "Constants.h"
#include "Mode.h"
#include "Timer.h"

#include <array>
#include <string>
#include <random>

const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_SIZE = 16;
const unsigned int FONT_SET_SIZE = 80;

class Chip8 {
public:
    explicit Chip8(Mode mode);

    void reset();

    void cycle();

    void loadRom(const std::string &filepath);

    std::array<uint8_t, KEY_COUNT> &keys();

    [[nodiscard]] const std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> &video() const;

    [[nodiscard]] bool drawFlag() const;

    void disableDrawFlag();

    [[nodiscard]] bool soundFlag() const;

    void disableSoundFlag();

private:
    void clearScreen();

    void decodeFuncTable0();

    void decodeFuncTable8();

    void decodeFuncTableE();

    void decodeFuncTableF();

    void opcodeUnknown();

    void opcode00E0();

    void opcode00EE();

    void opcode1NNN();

    void opcode2NNN();

    void opcode3XNN();

    void opcode4XNN();

    void opcode5XY0();

    void opcode6XNN();

    void opcode7XNN();

    void opcode8XY0();

    void opcode8XY1();

    void opcode8XY2();

    void opcode8XY3();

    void opcode8XY4();

    void opcode8XY5();

    void opcode8XY6();

    void opcode8XY7();

    void opcode8XYE();

    void opcode9XY0();

    void opcodeANNN();

    void opcodeBNNN();

    void opcodeCXNN();

    void opcodeDXYN();

    void opcodeEX9E();

    void opcodeEXA1();

    void opcodeFX07();

    void opcodeFX0A();

    void opcodeFX15();

    void opcodeFX18();

    void opcodeFX1E();

    void opcodeFX29();

    void opcodeFX33();

    void opcodeFX55();

    void opcodeFX65();

    std::array<uint8_t, MEMORY_SIZE> memory_;
    std::array<uint8_t, REGISTER_COUNT> registers_;
    uint16_t opcode_;
    uint16_t index_;
    uint16_t pc_;

    std::array<uint16_t, STACK_SIZE> stack_;
    uint16_t sp_;

    std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> video_; // uint32_t used to work well with SDL

    uint8_t delayTimer_;
    uint8_t soundTimer_;

    std::array<uint8_t, KEY_COUNT> keys_;

    bool drawFlag_;
    bool soundFlag_;

    const Mode mode_; // Specify whether to execute instructions like on the CHIP-8, CHIP-48 or SCHIP

    std::default_random_engine randEngine_;
    std::uniform_int_distribution<uint8_t> randByte_;

    Timer timer_;

    using chip8Func = void (Chip8::*)();
    chip8Func funcTable_[0xF + 1]{&Chip8::opcodeUnknown};
    chip8Func funcTable0_[0xE + 1]{&Chip8::opcodeUnknown};
    chip8Func funcTable8_[0xE + 1]{&Chip8::opcodeUnknown};
    chip8Func funcTableE_[0xE + 1]{&Chip8::opcodeUnknown};
    chip8Func funcTableF_[0x65 + 1]{&Chip8::opcodeUnknown};
};
