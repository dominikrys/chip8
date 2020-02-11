#pragma once

#include "Common.h"

#include <array>
#include <cstdint>

class KeyboardHandler {
public:
    explicit KeyboardHandler(std::array<uint8_t, KEY_COUNT>& keys);

    [[nodiscard]] bool handle();

private:
    std::array<uint8_t, KEY_COUNT> &keys_;
};
