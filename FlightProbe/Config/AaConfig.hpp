#pragma once

#include "aardvark.h"
#include <cstdint>

namespace AaConfig
{
    inline constexpr std::uint16_t API_VERSION = 0x0600; // v6.00
    inline constexpr AardvarkConfig CONFIG = AA_CONFIG_GPIO_I2C;
    inline constexpr int BITRATE = 100; // kilohertz
    inline constexpr int PULLUP = AA_I2C_PULLUP_NONE;

    inline constexpr std::uint32_t ID_PFWD = 0x855c182c;
    inline constexpr std::uint32_t ID_P45 = 0x855b0587;
    inline constexpr std::uint32_t ID_PST_IMU = 0x855c7344;
}
