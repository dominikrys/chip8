#include <algorithm>
#include <iostream>
#include <charconv>
#include "Configurator.h"

Configurator::Configurator(int &argc, char **argv) {
    for (int i = 1; i < argc; i++)
    {
        tokens_.emplace_back(argv[i]);
    }

}

std::string Configurator::getArgValue(const std::string &option) {
    std::vector<std::string>::const_iterator itr;

    itr = std::find(tokens_.begin(), tokens_.end(), option);

    if (itr != tokens_.end() && ++itr != tokens_.end())
    {
        return *itr;
    }

    return "";
}

bool Configurator::argExists(const std::string &option) {
    return std::find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
}

bool Configurator::configure(Config &config) {
    config.romPath_ = getArgValue("--rom");
    if (config.romPath_.empty())
    {
        return false;
    }

    std::string videoScaleStr = getArgValue("--scale");
    if (!videoScaleStr.empty())
    {
        std::from_chars(videoScaleStr.data(), videoScaleStr.data() + videoScaleStr.size(), config.videoScale_);
    }

    std::string cycleDelayStr = getArgValue("--delay");
    if (!cycleDelayStr.empty())
    {
        std::from_chars(cycleDelayStr.data(), cycleDelayStr.data() + cycleDelayStr.size(), config.cycleDelay_);
    }

    if (argExists("--quirk"))
    {
        // TODO: Implement this
    }

    if (argExists("--mute"))
    {
        config.mute_ = true;
    }

    return true;
}
