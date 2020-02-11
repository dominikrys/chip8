#pragma once

const unsigned int KEY_COUNT = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

enum class Mode {
    CHIP8, CHIP48, SCHIP // NOTE: SCHIP opcodes aren't actually implemented. Check the README for an explanation.
};
