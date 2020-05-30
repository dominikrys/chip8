#pragma once

#include "Constants.h"
#include "Mode.h"

#include <string>

struct Config {
    Config() : romPath_{}, videoScale_{15}, cpuFrequency_{1000}, mute_{false}, mode_{Mode::SCHIP} {}

    std::string romPath_;
    int videoScale_;
    int cpuFrequency_;
    bool mute_;
    Mode mode_;
};
