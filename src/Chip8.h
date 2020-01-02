#pragma once

#include <string>
#include <random>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_SIZE = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int FONT_SET_SIZE = 80;

class Chip8 {
public:
    explicit Chip8(bool altOp);

    void cycle();

    uint8_t *keys();

    void loadRom(const std::string &filepath);

    uint32_t *video();

    bool drawFlag();

    void disableDrawFlag();

    bool soundFlag();

    void disableSoundFlag();

private:
    void clearScreen();

    uint8_t memory_[MEMORY_SIZE]{};
    uint8_t registers_[REGISTER_COUNT]{};

    uint16_t opcode_{};
    uint16_t index_{};
    uint16_t pc_{};

    uint32_t video_[VIDEO_WIDTH * VIDEO_HEIGHT]{}; // uint32_t used to work with SDL well

    uint8_t delayTimer_{};
    uint8_t soundTimer_{};

    uint16_t stack_[STACK_SIZE]{};
    uint16_t sp_{};

    uint8_t keys_[KEY_COUNT]{};

    uint8_t fontSet_[FONT_SET_SIZE] =
            {
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

    bool drawFlag_{};
    bool soundFlag_{};

    bool altOp_{};

    std::default_random_engine randGen_;
    std::uniform_int_distribution<uint8_t> randByte_;
};