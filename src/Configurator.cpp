#include "Configurator.h"

#include <algorithm>
#include <iostream>
#include <charconv>

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
              "   --delay <delay>         Set delay between cycles in milliseconds. Floats accepted. Default: " +
              std::to_string(defaultConfig.cycleDelay_) + "\n" \
              "   --mute                  Disable sound. Default: " << defaultConfig.mute_ << "\n" \
              "   --altop                 Increment the index after executing the 8XY6 and 8XYE opcodes, as on the" \
              "                           original CHIP-8 and CHIP-48. This may help certain games. Default: "
              << defaultConfig.altOp_ << "\n";
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
    if (config.romPath_ = getArgValue("--rom"); config.romPath_.empty())
    {
        return false;
    }

    if (std::string videoScaleStr = getArgValue("--scale"); !videoScaleStr.empty())
    {
        auto result = std::from_chars(videoScaleStr.data(), videoScaleStr.data() + videoScaleStr.size(),
                                      config.videoScale_);

        if (!(bool) result.ec && result.ptr == videoScaleStr.data() + videoScaleStr.size())
        {
            std::cout << "Couldn't convert given scale value to int, using the default instead: " +
                         std::to_string(config.videoScale_);
        }
    }

    if (std::string cycleDelayStr = getArgValue("--delay"); !cycleDelayStr.empty())
    {
        // NOTE: can't use from_chars as above due to MinGW not yet supporting it for floats.
        try
        {
            config.cycleDelay_ = std::stod(cycleDelayStr, nullptr);
        }
        catch (const std::exception &e)
        {
            std::cout << "Couldn't convert given cycle delay value to double, using the default instead: " +
                         std::to_string(config.cycleDelay_);
        }
    }

    if (argExists("--altop"))
    {
        config.altOp_ = true;
    }

    if (argExists("--mute"))
    {
        config.mute_ = true;
    }

    return true;
}
