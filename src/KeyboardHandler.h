#pragma once

#include <cstdint>

class KeyboardHandler {
public:
    explicit KeyboardHandler(uint8_t *keys);

    bool handle();

private:
    uint8_t *keys{};
};