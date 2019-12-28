#include "Audio.h"

Audio::Audio() : amplitude{28000}, sampleRate{44100} {
    auto check43 = SDL_Init(SDL_INIT_AUDIO); // TODO: check error

    audioCallback.sampleRate = sampleRate;
    audioCallback.amplitude = amplitude;

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = sampleRate;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = AudioCallback::forwardCallback;
    desiredSpec.userdata = &audioCallback;


    auto check = SDL_OpenAudio(&desiredSpec, &audioSpec); // TODO: check error
    auto check2 = desiredSpec.format != audioSpec.format;
    auto check3 = 2;
}

Audio::~Audio() {
    SDL_CloseAudio();
}

void Audio::play() {
    SDL_PauseAudio(0); // start playing sound
    SDL_Delay(1000); // wait while sound is playing
    SDL_PauseAudio(1); // stop playing sound
}
