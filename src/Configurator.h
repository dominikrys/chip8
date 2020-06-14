#pragma once

#include "Config.h"
#include "Mode.h"

#include <string>
#include <unordered_map>
#include <vector>

class Configurator {
public:
    Configurator(int &argc, char **argv);

    void printUsage();

    void configure(Config &config);

private:
    [[nodiscard]] std::string getArgValue(const std::string &option) const;

    [[nodiscard]] bool argExists(const std::string &option) const;

    std::string modeToStr(Mode mode);

    Mode strToMode(const std::string &str, Mode defaultMode);

    std::string programName_;
    std::vector<std::string> tokens_;
    std::unordered_map<Mode, std::string> modeMap_;
};
