#include "SDL2/SDL.h"
#include <chrono>
#include "KeyboardHandler.h"
#include "Chip8.h"

const int VIDEO_SCALE = 15;

int main() {
    SDL_Init(SDL_INIT_VIDEO); // todo: check error

    // TODO: refactor SDL stuff out
    auto window = SDL_CreateWindow("CHIP-8 Emulator8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE, SDL_WINDOW_SHOWN);

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH,
                                     VIDEO_HEIGHT);

    Chip8 chip8{};
    KeyboardHandler keyboardHandler(chip8.getKeys());

    chip8.loadGame("bin/games/spaceinvaders.ch8");

    //TODO: add destructor for these

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
                auto pitch = sizeof(chip8.getVideo()[0]) * VIDEO_WIDTH;
                SDL_UpdateTexture(texture, nullptr, chip8.getVideo(), pitch);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);

                chip8.disableDrawFlag();
            }
        }
    }

    return 0;
}