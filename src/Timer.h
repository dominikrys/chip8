#pragma once

#include <chrono>

namespace chrono = std::chrono;
using high_resolution_clock = chrono::high_resolution_clock;

class Timer
{
public:
    Timer(double interval) : lastUpdate_{chrono::high_resolution_clock::now()}, interval_{interval} {}

    bool intervalElapsed()
    {
        const auto deltaTime = high_resolution_clock::now() - lastUpdate_;
        if (chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() > interval_)
        {
            lastUpdate_ = high_resolution_clock::now();

            return true;
        }
        else
        {
            return false;
        }
    }

private:
    std::chrono::high_resolution_clock::time_point lastUpdate_;
    double interval_;
};