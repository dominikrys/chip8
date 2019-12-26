#pragma once

#include "SDL2/SDL.h"

#include <string>

class Renderer {
public:
    Renderer(const char *title, int videoWidth, int videoHeight, int videoScale);

    ~Renderer();

    void Update(const void *pixels, int pitch);

private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
};