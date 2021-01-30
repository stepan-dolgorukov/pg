#pragma once

#include <cstdint>
#include <cstddef>

// ID контролов:
enum CONTROLS_IDS : std::uint8_t
{
    CLOSE_BUTTON    = 1u,
    MINIMIZE_BUTTON = 2u,
    GENERATE_BUTTON = 8u,
    INFO_BUTTON     = 9u
};

static const std::size_t MIN_PASSWORD_LENGTH{ 4u };
static const std::size_t MAX_PASSWORD_LENGTH{ 256u };

static const std::size_t MIN_PASSWORDS_AMOUNT{ 1u };
static const std::size_t MAX_PASSWORDS_AMOUNT{ 4096u };