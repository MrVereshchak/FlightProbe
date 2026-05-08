#include "Sensor.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "SensorBus.hpp"

Sensor::Sensor(SensorBus *pBus) : pInterface(pBus)
{
    if (!pBus)
    {
        throw std::invalid_argument("SensorBus cannot be null\n");
    }
}

int Sensor::read(std::uint16_t address, std::uint16_t readSize, std::vector<std::uint8_t> &buffer)
{
    return pInterface->read(address, readSize, buffer);
}

int Sensor::write(std::uint16_t address, std::uint16_t writeSize, const std::vector<std::uint8_t> &buffer, I2cFlags flag)
{
    return pInterface->write(address, writeSize, buffer, flag);
}

std::string Sensor::getBusName()
{
    return pInterface->getBusName();
}

Sensor::~Sensor(void)
{
}