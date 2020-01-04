#pragma once

#include <string>
#include <vector>

struct Config {
    Config() : videoScale_{15}, cycleDelay_{0.5}, mute_{false}, altOp_{false} {}

    int videoScale_;
    double cycleDelay_;
    bool mute_;
    bool altOp_;
    std::string romPath_;
};

class Configurator {
public:
    Configurator(int &argc, char **argv);

    static void printUsage();

    bool configure(Config &config);

private:
    std::vector<std::string> tokens_;

    std::string getArgValue(const std::string &option);

    bool argExists(const std::string &option);
};