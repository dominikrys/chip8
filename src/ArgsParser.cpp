#include <algorithm>
#include "ArgsParser.h"

ArgsParser::ArgsParser(int &argc, char **argv) {
    for (int i = 1; i < argc; i++)
    {
        tokens.emplace_back(argv[i]);
    }
}

std::string ArgsParser::getCmdOption(const std::string &option) {
    std::vector<std::string>::const_iterator itr;

    itr = std::find(tokens.begin(), tokens.end(), option);

    if (itr != tokens.end() && ++itr != tokens.end())
    {
        return *itr;
    }

    return "";
}

bool ArgsParser::cmdOptionExists(const std::string &option) {
    return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
}