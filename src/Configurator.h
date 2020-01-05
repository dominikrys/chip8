#pragma once

#include "Mode.h"
#include <string>
#include <vector>

struct Config {
    Config() : romPath_{}, videoScale_{15}, cycleDelay_{0.5}, mute_{false}, mode_{Mode::SCHIP} {}

    std::string romPath_;
    int videoScale_;
    double cycleDelay_;
    bool mute_;
    Mode mode_;
};

class Configurator {
public:
    Configurator(int &argc, char **argv);

    void printUsage();

    bool configure(Config &config) const;

private:
    [[nodiscard]] std::string getArgValue(const std::string &option) const;

    [[nodiscard]] bool argExists(const std::string &option) const;

    std::string modeToStr(Mode mode);

    Mode strToMode(const std::string &str, Mode defaultMode) const;

    std::vector<std::string> tokens_;

    std::unordered_map<Mode, std::string> modeMap_;
};