#include "chip8.h"

#include <fstream>
#include <cstddef>
#include <cstring>
#include <vector>

Chip8::Chip8() {
    initialize();
};

void Chip8::initialize() {
    opcode = 0;
    I = 0;
    pc = 0x200;
    sp = 0;

    // TODO: Clear display, stack, memory, registers V[]

    // TODO: Load font set
    for (int i = 0; i < 80; i++) {
        memory[i] = fontSet[i];
    }

    // TODO: Reset timers
}

void Chip8::emulateCycle() {
    // TODO: Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];


    // TODO: Decode Opcode
    // TODO: Execute Opcode

    // TODO: Update timers
}

void Chip8::loadGame(const std::string &filepath) {
    // TODO: add more error checking. Result type.
    std::ifstream ifs(filepath, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Can't open file: " + filepath + ". " + std::strerror(errno));
    }

    ifs.seekg(0, std::ios::end);
    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    auto size = std::size_t(end - ifs.tellg());
    if (size == 0) {
        throw std::runtime_error("No game loaded");
    } else if (size > sizeof(memory) / sizeof(*memory)) {
        throw std::runtime_error("ROM too big!");
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    for (int i = 0; i < size; i++) {
        memory[i + 512] = buffer[i];
    }

    ifs.close();
}