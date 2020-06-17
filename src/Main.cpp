#include "Audio.h"
#include "Chip8.h"
#include "Config.h"
#include "Configurator.h"
#include "KeyboardHandler.h"
#include "Renderer.h"
#include "Timer.h"

#include <iostream>

int main(int argc, char **argv) {
    try
    {
        Configurator configurator{argc, argv};
        Config config{};
        configurator.configure(config);

        Chip8 chip8{config.mode_};
        chip8.loadRom(config.romPath_);

        KeyboardHandler keyboardHandler(chip8.keys());
        Renderer renderer{"CHIP-8 Emulator", VIDEO_WIDTH, VIDEO_HEIGHT, config.videoScale_};
        Audio audio{config.mute_};

        const double cycleDelay = (1.0 / config.cpuFrequency_) * 1000000000;
        Timer cycleTimer(cycleDelay);

        bool quit = false;

        while (!quit)
        {
            quit = keyboardHandler.handle();

            if (cycleTimer.intervalElapsed())
            {
                chip8.cycle();

                if (chip8.drawFlag())
                {
                    auto buffer = chip8.video();
                    renderer.update(buffer, sizeof(buffer[0]) * VIDEO_WIDTH);
                    chip8.disableDrawFlag();
                }
                else if (chip8.soundFlag())
                {
                    audio.play();
                    chip8.disableSoundFlag();
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what();
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
