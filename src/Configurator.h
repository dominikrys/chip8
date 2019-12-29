#pragma once

#include <string>
#include <vector>

struct Config {
    Config() : videoScale{15}, cycleDelay{1}, mute{false}, quirkMode{false} {}

    int videoScale;
    int cycleDelay;
    bool mute;
    bool quirkMode;
    std::string romPath;
};

class Configurator {
public:
    Configurator(int &argc, char **argv);

    bool configure(Config &config);

private:
    std::vector<std::string> tokens;

    std::string getCmdOption(const std::string &option);

    bool cmdOptionExists(const std::string &option);
};