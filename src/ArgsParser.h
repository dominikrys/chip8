#pragma once

#include <string>
#include <vector>

/*
 * Rudimentary command line argument parser based off https://stackoverflow.com/a/868894
 */
class ArgsParser {
public:
    ArgsParser(int &argc, char **argv);

    std::string getCmdOption(const std::string &option);

    bool cmdOptionExists(const std::string &option);

private:
    std::vector<std::string> tokens;
};