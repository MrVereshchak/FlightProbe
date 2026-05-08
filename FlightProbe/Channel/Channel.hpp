#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "ImuConfig.hpp"
#include "PressureConfig.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"

class Channel
{
public:
    // TODO: make private + print functions
    std::unique_ptr<SensorBus> pBus;
    std::vector<std::unique_ptr<Sensor>> pSensors;
    std::string chName;

    Channel(std::string name, std::unique_ptr<SensorBus> busParam);

    // Physical hardware is unique, so no copying, only moving
    Channel(const Channel &) = delete;
    Channel &operator=(const Channel &) = delete;

    Channel(Channel &&) = default;
    Channel &operator=(Channel &&) = default;

    std::uint32_t getAaId() const;

    void addPressureSensor(PressureConfig::Device configParam);
    void addIMUSensor(ImuConfig::Device configParam);

    ~Channel();
};