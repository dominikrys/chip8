#include "Renderer.h"

Renderer::Renderer(const char *title, int videoWidth, int videoHeight, int videoScale) {
    SDL_Init(SDL_INIT_VIDEO); // todo: check error

    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              videoWidth * videoScale, videoHeight * videoScale, SDL_WINDOW_SHOWN);

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, videoWidth,
                                 videoHeight);
}

Renderer::~Renderer() {
    SDL_DestroyTexture(texture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Renderer::update(const void *pixels, int pitch) {
    SDL_UpdateTexture(texture_, nullptr, pixels, pitch);
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}
