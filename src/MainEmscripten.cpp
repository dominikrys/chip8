#include "Audio.h"
#include "Chip8.h"
#include "Config.h"
#include "KeyboardHandler.h"
#include "Renderer.h"

#include <iostream>

#include <emscripten.h>

Config config{};
Chip8 chip8{config.mode_};
KeyboardHandler keyboardHandler(chip8.keys());
Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, config.videoScale_};
Audio audio{config.mute_};

// 10 cycles per tick seems approximately correspond to 750Hz on 60FPS display
const int cyclesPerTick = 10;

extern "C" {
void loadRom(char *path) {
    chip8.resetState();
    chip8.loadRom(path);
}

void stop() {
    chip8.resetState();
}
}

void mainLoop() {
    if (keyboardHandler.handle())
    {
        emscripten_cancel_main_loop();
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
    else if (chip8.soundFlag())
    {
        //audio.play(); TODO: add audio back in

        chip8.disableSoundFlag();
    }
}

int main() {
    try
    {
        std::string romPath = "bin/roms/revival/games/Pong [Paul Vervalin, 1990].ch8"; // TODO: remove this
        loadRom(romPath.data());

        emscripten_set_main_loop(mainLoop, 0, 1);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
