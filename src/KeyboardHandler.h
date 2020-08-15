#pragma once

#include "Constants.h"

#include <array>
#include <cstdint>

class KeyboardHandler {
public:
    explicit KeyboardHandler(std::array<uint8_t, KEY_COUNT> &keys);

    bool handle();

private:
    std::array<uint8_t, KEY_COUNT> &keys_;
};
