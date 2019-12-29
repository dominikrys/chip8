#include <SDL2/SDL.h>

class Audio {
public:
    explicit Audio(bool mute);

    ~Audio();

    void play();

private:
    static void audioCallback(void *data, Uint8 *buffer, int length);

    SDL_AudioDeviceID audioDevice;

    bool muted;
    const double sineFreq;
    const int sampleFreq;
    const double samplesPerSine;
    uint32_t samplePos;
};