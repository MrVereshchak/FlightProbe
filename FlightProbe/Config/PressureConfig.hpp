#pragma once

#include <cstdint>

namespace PressureConfig
{
    enum class SensorType : std::uint8_t
    {
        PFWD,
        P45,
        PSTAT

    };

    enum class Unit : std::uint8_t
    {
        PSI,
        BAR
    };

    struct Device
    {
        std::uint16_t         address;
        std::uint16_t         readSize;       // bytes
        float                 outputMin;
        float                 outputMax;
        float                 pressureMin;
        float                 pressureMax;
        int                   biasCount;      // Gen2-style raw count bias
        Unit                  pressureUnit;
        SensorType            sensorType;
    };

    // SSCSRNN1.6BA7A3
    constexpr Device SSCSRNN1_STAT =
    {
        0x0078,
        4,
        1638.0f,
        14746.0f,
        0.0f,
        1.6f,
        0,
        Unit::BAR,
        SensorType::PSTAT
    };

    // MS4525DO-DS5AI001DP
    constexpr Device MS4525DO_FWD =
    {
        0x0028,
        4,
        1638.0f,
        14746.0f,
        -1.0f,
        1.0f,
        7840,
        Unit::PSI,
        SensorType::PFWD
    };

    // MS4525DO-DS5AI001DP
    constexpr Device MS4525DO_45 =
    {
        0x0028,
        4,
        1638.0f,
        14746.0f,
        -1.0f,
        1.0f,
        7915,
        Unit::PSI,
        SensorType::P45
    };
}