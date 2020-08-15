#include "Audio.h"
#include "Chip8.h"
#include "Config.h"
#include "KeyboardHandler.h"
#include "Renderer.h"

#include <emscripten.h>

Config config{};
Chip8 chip8{config.mode_};
KeyboardHandler keyboardHandler(chip8.keys());
Renderer renderer{"WASM CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, 13};
int cyclesPerTick = 10;

extern "C" {
void loadRom(char *path, int cyclesPerTick_) {
    cyclesPerTick = cyclesPerTick_;

    chip8.reset();
    chip8.loadRom(path);
}

void stop() {
    emscripten_cancel_main_loop();

    chip8.reset();
    auto buffer = chip8.video();
    renderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);
}
}

void mainLoop() {
    if (keyboardHandler.handle())
    {
        stop();
    }

    for (int i = 0; i < cyclesPerTick; i++)
    {
        chip8.cycle();
    }

    if (chip8.drawFlag())
    {
        auto buffer = chip8.video();
        renderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);
        chip8.disableDrawFlag();
    }
}

int main() {
    emscripten_set_main_loop(mainLoop, 0, 0);

    return EXIT_SUCCESS;
}