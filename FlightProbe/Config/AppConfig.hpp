#pragma once

#include <cstddef>

namespace Config
{
    inline constexpr bool DEBUG_MODE = false;
    inline constexpr bool DISPLAY_MODE = false;
    inline constexpr bool CALIBRATION_MODE = false;
    inline constexpr std::uint32_t CALIBRATION_COUNT = 500;
    inline constexpr int EXPECTED_NUM_OF_DEVICES = 3;
}