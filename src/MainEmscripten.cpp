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

// 10 cycles per tick seems to more or less correspond to 750Hz on 60FPS display
const int cyclesPerTick = 10;

void loopFunction() {
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
    config.romPath_ = "../bin/roms/revival/games/Pong [Paul Vervalin, 1990].ch8";

    chip8.loadRom(config.romPath_);

    emscripten_set_main_loop(loopFunction, 0, 1);
}
