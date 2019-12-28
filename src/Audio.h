#include <SDL2/SDL.h>

class Audio {
public:
    Audio();

    ~Audio();

    void play();

    const double sineFreq;
    const int sampleFreq;
    const double samplesPerSine;
    uint32_t samplePos;

private:
    static void audioCallback(void *data, Uint8 *buffer, int length);

    SDL_AudioDeviceID audioDevice;
};