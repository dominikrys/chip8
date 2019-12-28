#include "Audio.h"
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>

Audio::Audio()
        : sineFreq{400},
          sampleFreq{44100},
          samplesPerSine{sampleFreq / sineFreq},
          samplePos{0} {
    SDL_Init(SDL_INIT_AUDIO); // TODO: check error

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;

    SDL_zero(desiredSpec);
    desiredSpec.freq = sampleFreq;
    desiredSpec.format = AUDIO_U8;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;

    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec,
                                      SDL_AUDIO_ALLOW_FORMAT_CHANGE); // TODO:  check error
}

Audio::~Audio() {
    SDL_CloseAudioDevice(audioDevice);
    SDL_CloseAudio();
}

void Audio::play() {
    auto playFunc = [&]()
    {
        int duration = 75;
        bool playing = true;
        SDL_PauseAudioDevice(audioDevice, 0);

        auto startCycleTime = std::chrono::high_resolution_clock::now();
        while (playing)
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - startCycleTime).count();
            if (timeSinceStart > duration)
            {
                SDL_PauseAudioDevice(audioDevice, 1);
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
        buffer[i] = (std::sin(audio->samplePos / audio->samplesPerSine * M_PI * 2) + 1) * 127.5;
        ++audio->samplePos;
    }
}
