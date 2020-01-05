#include "Configurator.h"

#include <algorithm>
#include <charconv>
#include <iostream>

Configurator::Configurator(int &argc, char **argv) {
    for (int i = 1; i < argc; i++)
    {
        tokens_.emplace_back(argv[i]);
    }

    modeMap_ = {{Mode::CHIP8,  "8"},
                {Mode::CHIP48, "48"},
                {Mode::SCHIP,  "S"},
    };
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
              "   --mode ( 8 | 48 | S )   Choose the way opcodes 8XY6, 8XYE, FX55 and FX65 are executed.         \n\n" \
              "                           8: execute like on the original CHIP-8. Most games won't work properly.  \n" \
              "                           but this can help very old games.                                      \n\n" \
              "                           48: execute like on the CHIP-48. Most games will work properly.        \n\n" \
              "                           S: execute like on the SCHIP (without SCHIP opcode support). The majority\n" \
              "                           of games assume that the FX55 and FX65 will work like on the SCHIP.      \n" \
              "                           Default: " + modeToStr(defaultConfig.mode_) + "\n";
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

    if (argExists("--mute"))
    {
        config.mute_ = true;
    }

    if (std::string modeStr = getArgValue("--mode"); !modeStr.empty())
    {
        config.mode_ = strToMode(modeStr, config.mode_);
    }

    return true;
}

std::string Configurator::getArgValue(const std::string &option) const {
    std::vector<std::string>::const_iterator it{std::find(tokens_.begin(), tokens_.end(), option)};

    if (it != tokens_.end() && ++it != tokens_.end())
    {
        return *it;
    }

    return "";
}

bool Configurator::argExists(const std::string &option) const {
    return std::find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
}

std::string Configurator::modeToStr(Mode mode) {
    if (modeMap_.find(mode) != modeMap_.end())
    {
        return modeMap_[mode];
    }
    else
    {
        // This will only occur if the modeMap is not updated after a new mode is added
        return "Unknown";
    }
}

Mode Configurator::strToMode(const std::string &str, Mode defaultMode) {
    for (const auto &it : modeMap_)
    {
        if (it.second == str)
        {
            return it.first;
        }
    }

    std::cout << "Specified mode not found, using default instead: " + modeToStr(defaultMode);
    return defaultMode;
}
