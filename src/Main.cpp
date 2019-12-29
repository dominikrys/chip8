#include "KeyboardHandler.h"
#include "Chip8.h"
#include "Renderer.h"
#include "Configurator.h"
#include "Audio.h"
#include <chrono>
#include <iostream>

void printUsage() {
    Config defaultConfig{};
    std::cout << std::boolalpha <<
              "Usage:                                                                                              \n" \
              "   --rom <path>            Load ROM from specified path.                                            \n" \
              "                                                                                                    \n" \
              "Optional:                                                                                           \n" \
              "   --scale <scale factor>  Set scale factor of the window. Default: " +
              std::to_string(defaultConfig.videoScale) + "\n" \
              "   --delay <delay>         Set delay between cycles in milliseconds. Default: " +
              std::to_string(defaultConfig.cycleDelay) + "\n" \
              "   --mute                  Disable sound. Default: " << defaultConfig.mute << "\n" \
              "   --altop                 Increment the index after executing the 8XY6 and 8XYE opcodes, as on the" \
              "                           original CHIP-8 and CHIP-48. This may help certain games, however should be " \
              "                           left disabled for SCHIP games Default: "
              << defaultConfig.altOp << "\n";
    // TODO: add SCHIP or update this comment
}

int main(int argc, char **argv) {
    Config config{};
    Configurator configurator{argc, argv};

    if (!configurator.configure(config))
    {
        printUsage();
        std::exit(EXIT_FAILURE);
    }

    Chip8 chip8{config.altOp};
    chip8.loadRom(config.romPath);

    KeyboardHandler keyboardHandler(chip8.getKeys());
    Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, config.videoScale};
    Audio audio{config.mute};

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    bool quit = false;

    while (!quit)
    {
        quit = keyboardHandler.handle();

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeSinceLastCycle = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - lastCycleTime).count();

        if (timeSinceLastCycle >= config.cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.emulateCycle();

            if (chip8.getDrawFlag())
            {
                auto buffer = chip8.getVideo();
                renderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);

                chip8.disableDrawFlag();
            }

            if (chip8.getSoundFlag())
            {
                audio.play();

                chip8.disableSoundFlag();
            }
        }
    }

    return 0;
}