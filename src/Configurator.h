#pragma once

#include <string>
#include <vector>

struct Config {
    Config() : videoScale_{15}, cycleDelay_{0.5}, mute_{false}, altOp_{false}, romPath_{} {}

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

    bool configure(Config &config) const;

private:
    std::vector<std::string> tokens_;

    [[nodiscard]] std::string getArgValue(const std::string &option) const;

    [[nodiscard]] bool argExists(const std::string &option) const;
};