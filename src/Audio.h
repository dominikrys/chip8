#pragma once

#include <SDL2/SDL.h>

class Audio {
public:
    explicit Audio(bool mute);

    ~Audio();

    void play();

private:
    static void audioCallback(void *data, Uint8 *buffer, int length);

    SDL_AudioDeviceID audioDevice_;

    bool mute_;
    const double sineFreq_;
    const int sampleFreq_;
    const double samplesPerSine_;
    uint32_t samplePos_;
};
