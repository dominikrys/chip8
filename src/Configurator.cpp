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

void Configurator::printUsage() {
    Config defaultConfig{};
    std::cout << std::boolalpha <<
              "Usage:                                                                                              \n" \
              "   --rom <path>            Load ROM from specified path.                                            \n" \
              "                                                                                                    \n" \
              "Optional:                                                                                           \n" \
              "   --scale <scale factor>  Set scale factor of the window. Default: " +
              std::to_string(defaultConfig.videoScale_) + "\n" \
              "   --delay <delay>         Set delay between cycles in milliseconds. Default: " +
              std::to_string(defaultConfig.cycleDelay_) + "\n" \
              "   --mute                  Disable sound. Default: " << defaultConfig.mute_ << "\n" \
              "   --altop                 Increment the index after executing the 8XY6 and 8XYE opcodes, as on the" \
              "                           original CHIP-8 and CHIP-48. This may help certain games, however should be " \
              "                           left disabled for SCHIP games Default: "
              << defaultConfig.altOp_ << "\n";
    // TODO: add SCHIP or update this comment
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
