#include "Renderer.h"

#include <stdexcept>

Renderer::Renderer(const std::string &title, const int videoWidth, const int videoHeight, const int videoScale) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL video: " + std::string(SDL_GetError()));
    }

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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

void Renderer::update(const std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> &video, int pitch) const {
    SDL_UpdateTexture(texture_, nullptr, video.data(), pitch);
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}
