#include "Audio.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

Audio::Audio(bool mute)
        : mute_{mute},
          sineFreq_{500},
          sampleFreq_{32000},
          samplesPerSine_{sampleFreq_ / sineFreq_},
          samplePos_{0} {

    if (mute_) {
        return;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL audio: " + std::string(SDL_GetError()));
    }

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;

    SDL_zero(desiredSpec);
    desiredSpec.freq = sampleFreq_;
    desiredSpec.format = AUDIO_U8;
    desiredSpec.channels = 1;
    desiredSpec.samples = 512;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;

    audioDevice_ = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec,
                                       SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (!audioDevice_) {
        throw std::runtime_error("Failed to open SDL audio device: " + std::string(SDL_GetError()));
    }
}

Audio::~Audio() {
    if (!mute_) {
        SDL_CloseAudioDevice(audioDevice_);
        SDL_CloseAudio();
    }
}

void Audio::play() const {
    if (mute_) {
        std::cout << "BEEP\n";
        return;
    }

    auto playFunc = [&]() {
        int duration = 75;
        bool playing = true;
        SDL_PauseAudioDevice(audioDevice_, 0);

        auto startCycleTime = std::chrono::high_resolution_clock::now();
        while (playing) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - startCycleTime).count();
            if (timeSinceStart >= duration) {
                SDL_PauseAudioDevice(audioDevice_, 1);
                playing = false;
            }
        }
    };

    std::thread(playFunc).detach();
}

void Audio::audioCallback(void *data, Uint8 *buffer, int length) {
    auto *audio = reinterpret_cast<Audio *>(data);

    // Generate a simple sound wave
    for (int i = 0; i < length; i++) {
        buffer[i] = (std::sin(audio->samplePos_ / audio->samplesPerSine_ * M_PI * 2) + 1) * 127.5;
        ++audio->samplePos_;
    }
}
