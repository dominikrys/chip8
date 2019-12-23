#include "chip8.h"
#include <iostream>

int main() {
    // Set up renderer and register input callbacks
    //initialiseGraphics();
    //intialiseInput();

    Chip8 chip8{};
    chip8.loadGame("C:\\projects\\chip-8\\bin\\games\\pong.ch8");

    for (;;)
    {
        // Emulate one CPU cycle
        chip8.emulateCycle();

        if (chip8.drawFlag())
        {
            //drawScreen();
        }
    }

    //chip8.setKeys()

    return 0;
}