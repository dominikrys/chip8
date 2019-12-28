#include "KeyboardHandler.h"
#include "Chip8.h"
#include "Renderer.h"
#include "ArgsParser.h"
#include "Audio.h"
#include <chrono>
#include <iostream>
#include <charconv>

void printUsage(const std::string &videoScale, const std::string &cycleDelay, bool quirkMode) {
    std::cout << std::boolalpha <<
            "Usage:                                                                                                \n" \
            "   --rom <path>            Load ROM from specified path.                                              \n" \
            "                                                                                                      \n" \
            "Optional:                                                                                             \n" \
            "   --scale <scale factor>  Set scale factor of the window. Default: " + videoScale + "\n" \
            "   --delay <delay>         Set delay between cycles in milliseconds. Default: " + cycleDelay + "\n" \
            "   --quirk                 Enable mode for alternate opcode behaviour. Default: " << quirkMode << "\n";
}

int main(int argc, char **argv) {
    int videoScale = 15;
    int cycleDelay = 0;
    bool quirkMode = false;

    ArgsParser argsParser{argc, argv};

    std::string romPath = argsParser.getCmdOption("--rom");
    if (romPath.empty())
    {
        printUsage(std::to_string(cycleDelay), std::to_string(videoScale), quirkMode);
        return 0;
    }

    std::string videoScaleStr = argsParser.getCmdOption("--scale");
    if (!videoScaleStr.empty())
    {
        std::from_chars(videoScaleStr.data(),videoScaleStr.data() + videoScaleStr.size(), videoScale);
    }

    std::string cycleDelayStr = argsParser.getCmdOption("--delay");
    if (!cycleDelayStr.empty())
    {
        std::from_chars(cycleDelayStr.data(),cycleDelayStr.data() + cycleDelayStr.size(), cycleDelay);
    }

    if (argsParser.cmdOptionExists("--quirk"))
    {
        // TODO: Implement this
    }

    Chip8 chip8{};
    chip8.loadRom(romPath);

    KeyboardHandler keyboardHandler(chip8.getKeys());

    Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, videoScale};

    Audio audio{};

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    bool quit = false;

    while (!quit)
    {
        quit = keyboardHandler.handle();

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeSinceLastCycle = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - lastCycleTime).count();

        if (timeSinceLastCycle > cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.emulateCycle();

            if (chip8.getDrawFlag())
            {
                auto buffer = chip8.getVideo();
                renderer.Update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);

                chip8.disableDrawFlag();
            }

            if (chip8.getSoundFlag())
            {
                // Play sound
                audio.play();

                chip8.disableSoundFlag();
            }
        }
    }

    return 0;
}