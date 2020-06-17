#include "Audio.h"
#include "Chip8.h"
#include "Config.h"
#include "KeyboardHandler.h"
#include "Renderer.h"

#include <emscripten.h>

#include <iostream>

Config kConfig{};
Chip8 kChip8{kConfig.mode_};
KeyboardHandler kKeyboardHandler(kChip8.keys());
Renderer kRenderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, kConfig.videoScale_};
int kCyclesPerTick = 10;

extern "C" {
void loadRom(char *path, int cyclesPerTick) {
    kCyclesPerTick = cyclesPerTick;

    kChip8.resetState();
    kChip8.loadRom(path);
}

void stop() {
    emscripten_cancel_main_loop();

    kChip8.resetState();
    auto buffer = kChip8.video();
    kRenderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);
}
}

void mainLoop() {
    if (kKeyboardHandler.handle())
    {
        stop();
    }

    for (int i = 0; i < kCyclesPerTick; i++)
    {
        kChip8.cycle();
    }

    if (kChip8.drawFlag())
    {
        auto buffer = kChip8.video();
        kRenderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);
        kChip8.disableDrawFlag();
    }
}

int main() {
    try
    {
        emscripten_set_main_loop(mainLoop, 0, 0);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
