#pragma once

enum class Mode {
    // NOTE: SCHIP opcodes aren't implemented, this is so that FX55 and FX65 opcodes work like on the SCHIP.
    CHIP8, CHIP48, SCHIP
};
