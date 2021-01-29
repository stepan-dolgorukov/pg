#pragma once

#include <cstdint>

// ID контролов:
enum CONTROLS_IDS : std::uint8_t
{
    CLOSE_BUTTON    = 1u,
    MINIMIZE_BUTTON = 2u,
    GENERATE_BUTTON = 8u,
    INFO_BUTTON     = 9u
};