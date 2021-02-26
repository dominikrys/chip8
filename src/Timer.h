#pragma once

#include <chrono>

namespace chrono = std::chrono;
using high_resolution_clock = chrono::high_resolution_clock;

class Timer {
public:
    explicit Timer(double intervalNs) : lastUpdate_{high_resolution_clock::now()}, intervalNs_{intervalNs} {}

    bool intervalElapsed() {
        const auto deltaTime = high_resolution_clock::now() - lastUpdate_;

        if (chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() > intervalNs_) {
            lastUpdate_ = high_resolution_clock::now();

            return true;
        } else {
            return false;
        }
    }

private:
    std::chrono::high_resolution_clock::time_point lastUpdate_;
    double intervalNs_; // Can't use std::chrono here in case a value which is not known at compile time is passed in.
};