#include "chip8.h"
#include "SDL2/SDL.h"
#include <chrono>

const int VIDEO_SCALE = 15;

bool processInput(uint8_t *keys) {
    /*
    Keypad       Keyboard
    +-+-+-+-+    +-+-+-+-+
    |1|2|3|C|    |1|2|3|4|
    +-+-+-+-+    +-+-+-+-+
    |4|5|6|D|    |Q|W|E|R|
    +-+-+-+-+ => +-+-+-+-+
    |7|8|9|E|    |A|S|D|F|
    +-+-+-+-+    +-+-+-+-+
    |A|0|B|F|    |Z|X|C|V|
    +-+-+-+-+    +-+-+-+-+
     */
    bool quit = false;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_1:
                        keys[0x1] = 1;
                        break;
                    case SDLK_2:
                        keys[0x2] = 1;
                        break;
                    case SDLK_3:
                        keys[0x3] = 1;
                        break;
                    case SDLK_4:
                        keys[0xC] = 1;
                        break;
                    case SDLK_q:
                        keys[0x4] = 1;
                        break;
                    case SDLK_w:
                        keys[0x5] = 1;
                        break;
                    case SDLK_e:
                        keys[0x6] = 1;
                        break;
                    case SDLK_r:
                        keys[0xD] = 1;
                        break;
                    case SDLK_a:
                        keys[0x7] = 1;
                        break;
                    case SDLK_s:
                        keys[0x8] = 1;
                        break;
                    case SDLK_d:
                        keys[0x9] = 1;
                        break;
                    case SDLK_f:
                        keys[0xE] = 1;
                        break;
                    case SDLK_z:
                        keys[0xA] = 1;
                        break;
                    case SDLK_x:
                        keys[0x0] = 1;
                        break;
                    case SDLK_c:
                        keys[0xB] = 1;
                        break;
                    case SDLK_v:
                        keys[0xF] = 1;
                        break;
                }
                break;
            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_1:
                        keys[0x1] = 0;
                        break;
                    case SDLK_2:
                        keys[0x2] = 0;
                        break;
                    case SDLK_3:
                        keys[0x3] = 0;
                        break;
                    case SDLK_4:
                        keys[0xC] = 0;
                        break;
                    case SDLK_q:
                        keys[0x4] = 0;
                        break;
                    case SDLK_w:
                        keys[0x5] = 0;
                        break;
                    case SDLK_e:
                        keys[0x6] = 0;
                        break;
                    case SDLK_r:
                        keys[0xD] = 0;
                        break;
                    case SDLK_a:
                        keys[0x7] = 0;
                        break;
                    case SDLK_s:
                        keys[0x8] = 0;
                        break;
                    case SDLK_d:
                        keys[0x9] = 0;
                        break;
                    case SDLK_f:
                        keys[0xE] = 0;
                        break;
                    case SDLK_z:
                        keys[0xA] = 0;
                        break;
                    case SDLK_x:
                        keys[0x0] = 0;
                        break;
                    case SDLK_c:
                        keys[0xB] = 0;
                        break;
                    case SDLK_v:
                        keys[0xF] = 0;
                        break;
                }
                break;
            }
        }
    }

    return quit;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO); // todo: check error

    // TODO: refactor SDL stuff out
    auto window = SDL_CreateWindow("CHIP-8 Emulator8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE, SDL_WINDOW_SHOWN);

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH,
                                     VIDEO_HEIGHT);
    Chip8 chip8{};
    chip8.loadGame("bin/games/pong.ch8");

    int cycleDelay = 0;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    bool quit = false;

    while (!quit)
    {
        quit = processInput(chip8.getKeys());

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