#include "Audio.h"

Audio::Audio()  {

}

Audio::~Audio() {
    SDL_CloseAudio();
}

void Audio::play() {
    SDL_PauseAudio(0); // start playing sound
    SDL_Delay(1000); // wait while sound is playing
    SDL_PauseAudio(1); // stop playing sound
}
