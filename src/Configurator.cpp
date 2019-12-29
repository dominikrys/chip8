#include <algorithm>
#include <iostream>
#include <charconv>
#include "Configurator.h"

Configurator::Configurator(int &argc, char **argv) {
    for (int i = 1; i < argc; i++)
    {
        tokens.emplace_back(argv[i]);
    }

}

std::string Configurator::getCmdOption(const std::string &option) {
    std::vector<std::string>::const_iterator itr;

    itr = std::find(tokens.begin(), tokens.end(), option);

    if (itr != tokens.end() && ++itr != tokens.end())
    {
        return *itr;
    }

    return "";
}

bool Configurator::cmdOptionExists(const std::string &option) {
    return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
}

bool Configurator::configure(Config &config) {
    config.romPath = getCmdOption("--rom");
    if (config.romPath.empty())
    {
        return false;
    }

    std::string videoScaleStr = getCmdOption("--scale");
    if (!videoScaleStr.empty())
    {
        std::from_chars(videoScaleStr.data(), videoScaleStr.data() + videoScaleStr.size(), config.videoScale);
    }

    std::string cycleDelayStr = getCmdOption("--delay");
    if (!cycleDelayStr.empty())
    {
        std::from_chars(cycleDelayStr.data(), cycleDelayStr.data() + cycleDelayStr.size(), config.cycleDelay);
    }

    if (cmdOptionExists("--quirk"))
    {
        // TODO: Implement this
    }

    if (cmdOptionExists("--mute"))
    {
        config.mute = true;
    }

    return true;
}
