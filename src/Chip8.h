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

    void emulateCycle();

    uint8_t *getKeys();

    void loadRom(const std::string &filepath);

    uint32_t *getVideo();

    bool getDrawFlag();

    void disableDrawFlag();

    bool getSoundFlag();

    void disableSoundFlag();

private:
    void clearScreen();

    /*
    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM
    */
    uint8_t memory[MEMORY_SIZE]{};
    uint8_t registers[REGISTER_COUNT]{};

    uint16_t opcode{};
    uint16_t index{};
    uint16_t pc{};

    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{}; // uint32_t used to work with SDL well

    uint8_t delayTimer{};
    uint8_t soundTimer{};

    uint16_t stack[STACK_SIZE]{};
    uint16_t sp{};

    uint8_t keys[KEY_COUNT]{};

    uint8_t fontSet[FONT_SET_SIZE] =
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

    bool drawFlag{};

    bool soundFlag{};

    bool altOp{};

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
};