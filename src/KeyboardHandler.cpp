#include "KeyboardHandler.h"

#include <SDL2/SDL_events.h>

KeyboardHandler::KeyboardHandler(uint8_t *keys) : keys_{keys} {

}

bool KeyboardHandler::handle() {
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
                        keys_[0x1] = 1;
                        break;
                    case SDLK_2:
                        keys_[0x2] = 1;
                        break;
                    case SDLK_3:
                        keys_[0x3] = 1;
                        break;
                    case SDLK_4:
                        keys_[0xC] = 1;
                        break;
                    case SDLK_q:
                        keys_[0x4] = 1;
                        break;
                    case SDLK_w:
                        keys_[0x5] = 1;
                        break;
                    case SDLK_e:
                        keys_[0x6] = 1;
                        break;
                    case SDLK_r:
                        keys_[0xD] = 1;
                        break;
                    case SDLK_a:
                        keys_[0x7] = 1;
                        break;
                    case SDLK_s:
                        keys_[0x8] = 1;
                        break;
                    case SDLK_d:
                        keys_[0x9] = 1;
                        break;
                    case SDLK_f:
                        keys_[0xE] = 1;
                        break;
                    case SDLK_z:
                        keys_[0xA] = 1;
                        break;
                    case SDLK_x:
                        keys_[0x0] = 1;
                        break;
                    case SDLK_c:
                        keys_[0xB] = 1;
                        break;
                    case SDLK_v:
                        keys_[0xF] = 1;
                        break;
                }
                break;
            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_1:
                        keys_[0x1] = 0;
                        break;
                    case SDLK_2:
                        keys_[0x2] = 0;
                        break;
                    case SDLK_3:
                        keys_[0x3] = 0;
                        break;
                    case SDLK_4:
                        keys_[0xC] = 0;
                        break;
                    case SDLK_q:
                        keys_[0x4] = 0;
                        break;
                    case SDLK_w:
                        keys_[0x5] = 0;
                        break;
                    case SDLK_e:
                        keys_[0x6] = 0;
                        break;
                    case SDLK_r:
                        keys_[0xD] = 0;
                        break;
                    case SDLK_a:
                        keys_[0x7] = 0;
                        break;
                    case SDLK_s:
                        keys_[0x8] = 0;
                        break;
                    case SDLK_d:
                        keys_[0x9] = 0;
                        break;
                    case SDLK_f:
                        keys_[0xE] = 0;
                        break;
                    case SDLK_z:
                        keys_[0xA] = 0;
                        break;
                    case SDLK_x:
                        keys_[0x0] = 0;
                        break;
                    case SDLK_c:
                        keys_[0xB] = 0;
                        break;
                    case SDLK_v:
                        keys_[0xF] = 0;
                        break;
                }
                break;
            }
        }
    }

    return quit;
}