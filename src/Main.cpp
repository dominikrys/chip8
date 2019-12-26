#include "KeyboardHandler.h"
#include "Chip8.h"
#include "Renderer.h"
#include <chrono>

const int VIDEO_SCALE = 15;

int main() {
    Chip8 chip8{};
    chip8.loadGame("bin/games/spaceinvaders.ch8");

    KeyboardHandler keyboardHandler(chip8.getKeys());

    Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_SCALE};

    int cycleDelay = 0;

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
        }
    }

    return 0;
}