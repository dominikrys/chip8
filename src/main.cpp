#include "chip8.h"
#include "SDL2/SDL.h"
#include <chrono>

const int VIDEO_SCALE = 15;

int main() {
    SDL_Init(SDL_INIT_VIDEO); // todo: check error

    // TODO: refactor SDL stuff out
    auto window = SDL_CreateWindow("CHIP-8 Emulator8", 0, 0, VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE,
                                   SDL_WINDOW_SHOWN);

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH,
                                     VIDEO_HEIGHT);
    Chip8 chip8{};
    chip8.loadGame("bin/games/spaceinvaders.ch8");

    int cycleDelay = 05;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    //chip8.setKeys()

    for (;;)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeSinceLastCycle = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCycleTime).count();

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