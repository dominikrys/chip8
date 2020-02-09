#pragma once

#include "SDL2/SDL.h"

#include <string>

class Renderer {
public:
    Renderer(const std::string &title, int videoWidth, int videoHeight, int videoScale);

    ~Renderer();

    void update(const void *pixels, int pitch) const;

private:
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    SDL_Texture *texture_;
};
