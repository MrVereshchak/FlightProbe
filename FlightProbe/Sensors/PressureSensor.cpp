#include "PressureSensor.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

#include "PressureConfig.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"
#include "SensorResult.hpp"

PressureSensor::PressureSensor(SensorBus *pBus, PressureConfig::Device configParam)
    : Sensor(pBus),
      config(configParam)
{
}

SensorResult PressureSensor::getDecodedData()
{
    PressureResult results;
    std::vector<std::uint8_t> rawData(config.readSize, 0);

    int readStatus = Sensor::read(config.address, config.readSize, rawData);
    if (readStatus < 4)
    {
        std::cerr << "Invalid PS read ";
        return results;
    }


    // Decoding raw sensor data
    // Shift each byte into its correct "slot"
    std::uint32_t rawDataCombined = ((std::uint32_t)rawData[0] << 24) |
                                    ((std::uint32_t)rawData[1] << 16) |
                                    ((std::uint32_t)rawData[2] << 8) |
                                    (std::uint32_t)rawData[3];

    // Get status bits [0,2) = 2 bits
    std::uint8_t statusRaw = (rawDataCombined >> 30) & 0x0003;
    // Get pressure bits [2, 16) = 14 bits
    std::uint16_t pressureRaw = (rawDataCombined >> 16) & 0x3FFF & ~7;
    // Get temperature [16, 27) = 11 bits
    std::uint16_t temperatureRaw = (rawDataCombined >> 5) & 0x07FF;
    // Padding bits [27, 32)  = 5 bits

    results.rawCount = pressureRaw;
    results.correctedCount = (int)pressureRaw - config.biasCount;
    results.sensorType = config.sensorType;
    results.valid = (statusRaw == 0 || statusRaw == 2);

    // Status decoding
    switch (statusRaw)
    {
        case 0:
            results.statusStr = "Normal";
            break;
        case 1:
            results.statusStr = "Reserved";
            break; // should not be seen during normal operation
        case 2:
            results.statusStr = "Stale ";
            break;
        case 3:
            results.statusStr = "Fault ";
            break;
        default:
            results.statusStr = "Error ";
    }

    // Pressure decoding
    results.pressure = (((float)pressureRaw - config.outputMin) * (config.pressureMax - config.pressureMin)) / (config.outputMax - config.outputMin) + config.pressureMin;

    // Temperature decoding
    results.temperatureC = (((float)temperatureRaw * 200.0f) / 2047.0f) - 50.0f; // magic numbers from the datasheet
    results.temperatureF = (results.temperatureC * 1.8f) + 32;

    return results;
}

PressureSensor::~PressureSensor()
{
}