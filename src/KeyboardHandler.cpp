#include "KeyboardHandler.h"

#include <SDL2/SDL_events.h>

KeyboardHandler::KeyboardHandler(std::array<uint8_t, KEY_COUNT> &keys) : keys_{keys} {
}

bool KeyboardHandler::handle() {
    /*
    Chip-8
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
        int keyState = 0;

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                keyState = 1;
                break;
            case SDL_KEYUP:
            {
                keyState = 0;
                break;
            }
        }

        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                quit = true;
                break;
            case SDLK_1:
                keys_[0x1] = keyState;
                break;
            case SDLK_2:
                keys_[0x2] = keyState;
                break;
            case SDLK_3:
                keys_[0x3] = keyState;
                break;
            case SDLK_4:
                keys_[0xC] = keyState;
                break;
            case SDLK_q:
                keys_[0x4] = keyState;
                break;
            case SDLK_w:
                keys_[0x5] = keyState;
                break;
            case SDLK_e:
                keys_[0x6] = keyState;
                break;
            case SDLK_r:
                keys_[0xD] = keyState;
                break;
            case SDLK_a:
                keys_[0x7] = keyState;
                break;
            case SDLK_s:
                keys_[0x8] = keyState;
                break;
            case SDLK_d:
                keys_[0x9] = keyState;
                break;
            case SDLK_f:
                keys_[0xE] = keyState;
                break;
            case SDLK_z:
                keys_[0xA] = keyState;
                break;
            case SDLK_x:
                keys_[0x0] = keyState;
                break;
            case SDLK_c:
                keys_[0xB] = keyState;
                break;
            case SDLK_v:
                keys_[0xF] = keyState;
                break;
        }
    }

    return quit;
}
