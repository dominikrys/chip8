#include "chip8.h"
#include "SDL2/SDL.h"

const int MULTIPLIER = 10;

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("Chip8", 0, 0, 64 * MULTIPLIER, 32 * MULTIPLIER, SDL_WINDOW_SHOWN);

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    // Set up renderer and register input callbacks
    //initialiseGraphics();
    //intialiseInput();
    Chip8 chip8{};
    chip8.loadGame("bin/games/test_opcode.ch8");

    for (;;)
    {
        // Emulate one CPU cycle
        chip8.emulateCycle();

        if (chip8.drawFlag())
        {
            SDL_UpdateTexture(texture, nullptr, chip8.getGfx(), 32 * 8);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);

            //chip8.renderConsole();
            chip8.disableDrawFlag();
        }
    }

    //chip8.setKeys()

    return 0;
}