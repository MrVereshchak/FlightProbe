#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "SensorBus.hpp"
#include "SensorResult.hpp"

class Sensor
{
private:
    SensorBus *pInterface;

public:
    Sensor(SensorBus *pBus);

    // Block copying to prevent dangling pInterface pointers
    Sensor(const Sensor &) = delete;
    Sensor &operator=(const Sensor &) = delete;

    Sensor(Sensor &&) = default;
    Sensor &operator=(Sensor &&) = default;

    int read(std::uint16_t address, std::uint16_t readSize, std::vector<std::uint8_t> &buffer);
    int write(std::uint16_t address, std::uint16_t writeSize, const std::vector<std::uint8_t> &buffer, I2cFlags flag = I2cFlags::None);

    std::string getBusName();

    virtual SensorResult getDecodedData() = 0;

    virtual ~Sensor(void);
};