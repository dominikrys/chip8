#include "Audio.h"
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>

Audio::Audio(bool mute)
        : muted_{mute},
          sineFreq_{500},
          sampleFreq_{44100},
          samplesPerSine_{sampleFreq_ / sineFreq_},
          samplePos_{0} {

    if (muted_)
    {
        return;
    }

    SDL_Init(SDL_INIT_AUDIO); // TODO: check error

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;

    SDL_zero(desiredSpec);
    desiredSpec.freq = sampleFreq_;
    desiredSpec.format = AUDIO_U8;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;

    audioDevice_ = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec,
                                       SDL_AUDIO_ALLOW_FORMAT_CHANGE); // TODO:  check error
}

Audio::~Audio() {
    if (!muted_)
    {
        SDL_CloseAudioDevice(audioDevice_);
        SDL_CloseAudio();
    }
}

void Audio::play() {
    if (muted_)
    {
        std::cout << "BEEP\n";
        return;
    }

    auto playFunc = [&]()
    {
        int duration = 75;
        bool playing = true;
        SDL_PauseAudioDevice(audioDevice_, 0);

        auto startCycleTime = std::chrono::high_resolution_clock::now();
        while (playing)
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - startCycleTime).count();
            if (timeSinceStart >= duration)
            {
                SDL_PauseAudioDevice(audioDevice_, 1);
                playing = false;
            }
        }
    };

    std::thread(playFunc).detach();
}

void Audio::audioCallback(void *data, Uint8 *buffer, int length) {
    auto *audio = reinterpret_cast<Audio *>(data);

    for (int i = 0; i < length; i++)
    {
        buffer[i] = (std::sin(audio->samplePos_ / audio->samplesPerSine_ * M_PI * 2) + 1) * 127.5;
        ++audio->samplePos_;
    }
}
