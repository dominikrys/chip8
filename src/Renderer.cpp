#include "Renderer.h"

Renderer::Renderer(const char *title, int videoWidth, int videoHeight, int videoScale) {
    SDL_Init(SDL_INIT_VIDEO); // todo: check error

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              videoWidth * videoScale, videoHeight * videoScale, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, videoWidth,
                                videoHeight);
}

Renderer::~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::Update(const void *pixels, int pitch) {
    SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
