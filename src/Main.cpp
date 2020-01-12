#include "Audio.h"
#include "Chip8.h"
#include "Configurator.h"
#include "KeyboardHandler.h"
#include "Renderer.h"

#include <chrono>

int main(int argc, char **argv) {
    Config config{};
    Configurator configurator{argc, argv};
    if (!configurator.configure(config))
    {
        std::exit(EXIT_FAILURE);
    }

    Chip8 chip8{config.mode_};
    chip8.loadRom(config.romPath_);

    KeyboardHandler keyboardHandler(chip8.keys());
    Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, config.videoScale_};
    Audio audio{config.mute_};

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    bool quit = false;

    while (!quit)
    {
        quit = keyboardHandler.handle();

        // Check if enough time has passed since the previous emulation cycle to execute a new one. Nanoseconds used for
        // higher precision. The delay command line arg is in ms for ease of use.
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeSinceLastCycle = std::chrono::duration_cast<std::chrono::nanoseconds>(
                currentTime - lastCycleTime).count();

        auto cycleDelayNs = config.cycleDelay_ * 1000000;
        if (timeSinceLastCycle >= cycleDelayNs)
        {
            lastCycleTime = currentTime;

            chip8.cycle();

            if (chip8.drawFlag())
            {
                auto buffer = chip8.video();
                renderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);

                chip8.disableDrawFlag();
            }
            else if (chip8.soundFlag())
            {
                audio.play();

                chip8.disableSoundFlag();
            }
        }
    }

    return 0;
}