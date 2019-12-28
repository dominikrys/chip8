#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cmath>

struct AudioCallback {
    int sampleRate;
    int amplitude;

    void onCallback(Uint8 *raw_buffer, int bytes) {
        auto *buffer = (Sint16 *) raw_buffer;
        int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
        int sample_nr = 0;

        for (int i = 0; i < length; i++, sample_nr++)
        {
            double time = (double) sample_nr / (double) sampleRate;
            buffer[i] = (Sint16) (amplitude * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
        }
    }

    static void forwardCallback(void *userdata, Uint8 *stream, int len) {
        static_cast<AudioCallback *>(userdata)->onCallback(stream, len);
    }
};

class Audio {
public:
    //TODO: fix naming, imports, move stuff around and see what can be made private
    Audio();

    ~Audio();

    void play();

private:
    AudioCallback audioCallback;

    int amplitude{};
    int sampleRate{};

    SDL_AudioSpec audioSpec;
};