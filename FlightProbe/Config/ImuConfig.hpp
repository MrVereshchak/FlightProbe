#pragma once

#include <cstdint>

namespace ImuConfig
{
    struct Device
    {
        // Hardware addresses
        std::uint16_t agAddress;
        std::uint16_t magnAddress;

        // Who am I call configuration
        std::uint8_t whoAmIReg; // Who am I register address
        std::uint8_t agExpID;   // Expected ID return for accl/gyro after who am I call
        std::uint8_t magExpID;  // Expected ID return for magn after who am I call

        // Register addresses
        std::uint8_t acclReg;      // CTRL_REG6_XL sets data rate, scale, bandwidth
        std::uint8_t gyroReg;      // CTRL_REG1_G, sets data rate, scale, bandwidth
        std::uint8_t magnRegDr;    // CTRL_REG1_M, sets accuracy (operative) mode for X and Y and data rate for the whole sensor
        std::uint8_t magnRegSc;    // CTRL_REG2_M, sets scale
        std::uint8_t magnRegMd;    // CTRL_REG3_M, sets operation mode (Power-down, Continuous/Single-conversion)
        std::uint8_t magnRegPerfZ; // CTRL_REG4_M, sets accuracy performance (operative) mode for Z
        // accuracy mode means how much internal work the chip does to make that answer cleaner

        // Read register addresses
        std::uint8_t acclReadReg;
        std::uint8_t gyroReadReg;
        std::uint8_t magnReadReg;

        // Read size
        std::uint16_t readSize;

        // Initialization bytes (configuration values)
        std::uint8_t acclInitVal;
        std::uint8_t gyroInitVal;
        std::uint8_t magnInitValDr;
        std::uint8_t magnInitValSc;
        std::uint8_t magnInitValMd;
        std::uint8_t magnInitValPerfZ;

        // Scaling factors (sensitivities)
        float acclScale; // mg per bit
        float gyroScale; // dps per bit
        float magnScale; // mgauss per bit
    };

    constexpr Device LSM9DS1 = {
        0x006b, 0x001e,                     // Addresses
        0x0f, 0x68, 0x3d,                   // Who am I configuration
        0x20, 0x10, 0x20, 0x21, 0x22, 0x23, // Registers
        0x28, 0x18, 0x28, 0x06,             // Read register addresses, read size
        0x68, 0x60, 0xbc, 0x00, 0x00, 0x04, // Configuration codes 119Hz (accl/gyro), +/-16g (accl), 245dps (gyro), 80Hz, 4gauss
        0.732f, 0.00875f, 0.14f             // Sensitivity multipliers for +/-16g, 245dps, 4gauss
    };
}
