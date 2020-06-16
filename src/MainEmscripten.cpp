#include "Audio.h"
#include "Chip8.h"
#include "Config.h"
#include "KeyboardHandler.h"
#include "Renderer.h"

#include <iostream>

#include <emscripten.h>
#include <emscripten/bind.h>

Config config{};
Chip8 chip8{config.mode_};
KeyboardHandler keyboardHandler(chip8.keys());
Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, config.videoScale_};

// 10 cycles per tick seems approximately correspond to 750Hz on 60FPS display
const int cyclesPerTick = 10;

std::string getExceptionMessage(intptr_t exceptionPtr) {
    return std::string(reinterpret_cast<std::exception *>(exceptionPtr)->what());
}

EMSCRIPTEN_BINDINGS(Bindings) {
    emscripten::function("getExceptionMessage", &getExceptionMessage);
}

extern "C" {
void loadRom(char *path) {
    chip8.resetState();
    chip8.loadRom(path);
}

void stop() {
    emscripten_cancel_main_loop();

    chip8.resetState();

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
